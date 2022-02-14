#include "./internal/source/qsource_bridge.h"
#include <core/qtv_service.h>

extern const ui_prop_item bridge_encoder_param_struct[];

/* qbridge_avsource_property */

qbridge_avsource_property::qbridge_avsource_property(const Pui_prop_item uiprop, uint8_t* raw, qtvBridge* bridge) :
	qi_property(uiprop, raw),
	m_bridge(bridge)
{

}

bool qbridge_avsource_property::checkSelectValid(const Pui_prop_val val)
{
	if (m_bridge && m_bridge->_sources().count())
	{
		QList<Pbridge_source> sources = m_bridge->_sources();
		for (int32_t i = 0; i < sources.count(); i++)
		{
			Pbridge_source psource = sources.at(i);
			if (psource->source_id == val->val)
				return true;
		}
	}
	return false;
}

/* qbridge_encoder_properties */

qbridge_encoder_properties::qbridge_encoder_properties(qtvDevice* device) :
	m_bridge(device->_bridge()),
	m_device(device)
{
	m_param.vcodec = encvideo_mpeg2;
	m_param.acodec = encaudio_mp2;
	
	if (m_device->_chip_info()->chip_module == ic_module_b3_lite)
		m_param.vcodec = encvideo_h264;
	m_param.source = (bridge_tag_device)m_bridge->_sources().first()->source_id;
	m_param.pcrpid = 0x100;
	m_param.pmtpid = 0x1000;
	m_param.video_pid = 0x1001;
	m_param.audio_pid = 0x1002;
	resetProperties();
	refreshProperties();
}

void qbridge_encoder_properties::resetProperties()
{
	if(m_param.vcodec == encvideo_h264)
		memcpy(&m_param.quality, &def_quality_h264, sizeof(quality_param));
	else memcpy(&m_param.quality, &def_quality_mpeg2, sizeof(quality_param));
}

void qbridge_encoder_properties::refreshProperties()
{
	clearProperties();
	insertProperties(_ui_struct(bridge_encoder_param), (uint8_t*)&m_param);
	insertProperties(_ui_struct(quality_param), (uint8_t*)&m_param.quality);
}

void qbridge_encoder_properties::insertProperty(const Pui_prop_item uiprop, uint8_t* raw)
{
	if (ui_props_is_struct_param(bridge_encoder_param, source, uiprop))
		qi_properties_base::insertProperty(new qbridge_avsource_property(uiprop, raw, m_device->_bridge()));
	else if (ui_props_is_struct_param(bridge_encoder_param, vcodec, uiprop))
		qi_properties_base::insertProperty(new qi_property_video_codec(this, uiprop, raw, m_device));
	else qi_properties_base::insertProperty(uiprop, raw);
}

/* qbridge_encoder_factory */

qbridge_encoder_factory::qbridge_encoder_factory()
{
	m_name = "Bridge-AVsource";
}

qbridge_encoder_factory::~qbridge_encoder_factory()
{

}

bool qbridge_encoder_factory::checkSupported(qtvServiceBase* service)
{
	qtvBridge* bridge = service->_device()->_bridge();
	if (bridge)
	{
		if (bridge->_sources().count())
			return true;
	}
	return false;
}

vatek_result qbridge_encoder_factory::createProperties(qtvServiceBase* service, qtvPropertyCollection** properties)
{
	vatek_result nres = vatek_unsupport;
	if (service->_mode() == qservice_broadcast)
	{
		qtvDevice* device = service->_device();
		if (device->_bridge())
		{
			if (device->_bridge()->_sources().count())
			{
				*properties = new qbridge_encoder_properties(service->_device());
				nres = vatek_success;
			}
		}
	}
	return nres;
}

vatek_result qbridge_encoder_factory::checkProperties(qtvPropertyCollection* properties)
{
	return vatek_success;
}

vatek_result qbridge_encoder_factory::createSource(qtvServiceBase* service, qtvPropertyCollection* properties, qtvSourceBase** source)
{
	vatek_result nres = checkProperties(properties);
	if (is_vatek_success(nres))
	{
		qbridge_encoder_properties* qsource = static_cast<qbridge_encoder_properties*>(properties);
		if (qsource)*source = new qbridge_encoder_source(service, qsource, this);
		else nres = vatek_badparam;
	}
	return nres;
}

/* qbridge_encoder_source */

qbridge_encoder_source::qbridge_encoder_source(qtvServiceBase* service, qbridge_encoder_properties* properties, qtvSourceFactory* factory) :
	m_status(qstatus_idle),
	m_service(service),
	m_properties(properties),
	m_factory(factory),
	m_bsource(NULL),
	m_index(-1)
{
	m_device = m_service->_device();
	m_bridge = m_device->_bridge();
	resetParam(qstatus_unlock);
}

qbridge_encoder_source::~qbridge_encoder_source()
{
	delete m_properties;
}

vatek_result qbridge_encoder_source::open()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_idle)
	{
		bridge_encoder_param pbparam = m_properties->_bridge_param();
		int32_t i = 0;
		nres = vatek_success;
		for (i = 0; i < m_bridge->_sources().count(); i++)
		{
			if (m_bridge->_sources().at(i)->source_id == (bsource_id)pbparam.source)
			{
				m_index = i;
				m_bsource = m_bridge->_sources().at(i);
				break;
			}
		}
		if (m_bsource)m_status = qstatus_unlock;
		else nres = vatek_badparam;
	}
	return nres;
}

vatek_result qbridge_encoder_source::start()
{
	vatek_result nres = vatek_badstatus;
	if (m_bsource && m_status != qstatus_running)
	{
		qtv_status curstatus = checkSourceStatus();
		if (curstatus == qstatus_lock)
		{
			nres = m_bridge->startSource(m_bsource);
			if (is_vatek_success(nres))m_status = qstatus_running;
		}
	}
	return nres;
}

qtv_status qbridge_encoder_source::_status()
{
	if (m_bsource && m_status != qstatus_running)
		m_status = checkSourceStatus();
	return m_status;
}

Pqtv_source_param qbridge_encoder_source::_param()
{
	resetParam(checkSourceStatus());
	return &m_param;
}

vatek_result qbridge_encoder_source::polling()
{
	vatek_result nres = vatek_badstatus;
	if (m_status == qstatus_running)
	{
		qtv_status curstatus = checkSourceStatus();
		if (curstatus == qstatus_lock)nres = vatek_success;
	}
	return nres;
}

void qbridge_encoder_source::stop()
{
	if (m_status == qstatus_running)
	{
		m_bridge->stopSource(m_bsource);
		m_status = qstatus_unlock;
	}
}

void qbridge_encoder_source::close()
{
	m_status = qstatus_idle;
	m_bsource = NULL;
	m_index = -1;
}

qtv_status qbridge_encoder_source::checkSourceStatus()
{
	if (m_bsource)
	{
		vatek_result nres = vatek_success;
		if (m_status == qstatus_running)
			nres = m_bridge->checkSource(m_bsource);
		else nres = m_bridge->updateSource(m_index, m_bsource);

		if (is_vatek_success(nres))
		{
			if (m_bsource->status == bstatus_active_protect || m_bsource->status == bstatus_active)
				return qstatus_lock;
			else return qstatus_unlock;
		}
	}
	return qstatus_fail;
}

void qbridge_encoder_source::resetParam(qtv_status qstatus)
{
	Pencoder_param penc = &m_param.stream.encoder;
	bridge_encoder_param benc = m_properties->_bridge_param();
	memset(&m_param, 0, sizeof(qtv_source_param));
	m_param.filter = qfilter_disable;
	m_param.source = stream_source_encoder;
	memcpy(penc, &default_encoder_param, sizeof(encoder_param));

	penc->pmt_pid = benc.pmtpid;
	penc->audio_pid = benc.audio_pid;
	penc->video_pid = benc.video_pid;
	penc->mode = encoder_source_colorbar;
	memcpy(&penc->quality, &benc.quality, sizeof(quality_param));
	penc->audio.acodec = benc.acodec;
	penc->video.vcodec = benc.vcodec;

	if (qstatus == qstatus_lock)
	{
		penc->mode = encoder_source_vi_0;
		penc->video.resolution = m_bsource->video_info.resolution;
		penc->video.framerate = m_bsource->video_info.framerate;
		penc->video.aspectrate = m_bsource->video_info.aspectrate;
		penc->audio.channel = m_bsource->audio_info.channel;
		penc->audio.samplerate = m_bsource->audio_info.samplerate;
		penc->viparam.offset_x = m_bsource->video_info.offset_x;
		penc->viparam.offset_y = m_bsource->video_info.offset_y;
		penc->viparam.vi_flags = m_bsource->video_info.signal_flag;
		penc->viparam.vi_pixelclk = m_bsource->video_info.pixelclk;
	}
}

_ui_enum_start(bridge_tag_device)
	_val(bsource_tag_ep9555e, ep955e)
	_val(bsource_tag_ep9351, ep9351)
	_val(bsource_tag_adv718x, adv718x)
	_val(bsource_tag_sx1804a, sx1804a)
	_val(bdemod_tag_avl68xx, avl68xx)
	_val(brfmixer_tag_fintek_r2, fintek_r2)
	_val(brfmixer_tag_adrf6755, adrf6755)
_ui_enum_end

_ui_struct_start(bridge_encoder_param)
	_prop_h16(bridge_encoder_param, pcrpid, "pcr pid", NULLHAL)
	_prop_h16(bridge_encoder_param, pmtpid, "pmt pid", NULLHAL)
	_prop_h16(bridge_encoder_param, video_pid, "video pid", NULLHAL)
	_prop_h16(bridge_encoder_param, audio_pid, "audio pid", NULLHAL)
	_prop_enum(bridge_encoder_param, video_codec, vcodec, "video codec", NULLHAL)
	_prop_enum(bridge_encoder_param, audio_codec, acodec, "audio codec", NULLHAL)
	_prop_enum(bridge_encoder_param, bridge_tag_device, source, "source", NULLHAL)
_ui_struct_end