
#include <mux/mux_epg.h>

const uint8_t EIT_EVENT_JIS_NAME3[] = { 0x3C,0x2B,0x3C,0x67,0x4A,0x7C,0x41,0x77,0x23,0x32,0, };
const uint8_t EIT_EVENT_JIS_NAME4[] = { 0x3C,0x2B,0x3C,0x67,0x4A,0x7C,0x41,0x77,0x46,0x62,0x4d,0x46,0x23,0x32,0, };

#define EPG_SECTION_BASE_LEN	((EPG_TITLE_MAX_LEN + EPG_CONTENT_MAX_LEN) + sizeof(mux_epg_section))

extern Pmux_epg_section malloc_epg_section(uint8_t* pbuf, int32_t* pos);

vatek_result muxepg_create(Pth_mempool pmem, mux_epg_mode mode, Pmux_epg_param* pepg)
{
	int32_t len = sizeof(mux_epg_param);
	vatek_result nres = vatek_badparam;
	uint8_t* rawbuf = NULL;

	if (mode == epg_mode_dvb || mode == epg_mode_arib_0)len += EPG_SECTION_BASE_LEN;
	else if (mode == epg_mode_arib_1)len += EPG_SECTION_BASE_LEN * 2;
	else return nres;

	nres = vatek_memfail;
	rawbuf = th_mempool_malloc(pmem,len);

	if (rawbuf)
	{
		Pmux_epg_param pnewepg = (Pmux_epg_param)&rawbuf[0];
		len = sizeof(mux_epg_param);

		pnewepg->sections = malloc_epg_section(rawbuf, &len);
		if (mode == epg_mode_arib_1)
			pnewepg->sections->next = malloc_epg_section(rawbuf, &len);

		pnewepg->days = 8;
		pnewepg->epg_flags = 0;
		pnewepg->event_id = 0x2000;
		pnewepg->loop_ms = 30000;
		pnewepg->mode = mode;

		*pepg = pnewepg;
		nres = vatek_success;
	}
	return nres;
}

vatek_result muxepg_create_desc(Pth_mempool pmem, Pmux_epg_param pepg, Pmux_epg_desc* pdesc)
{
	Pmux_epg_desc pnewdesc = (Pmux_epg_desc)th_mempool_malloc(pmem, sizeof(mux_epg_desc));
	vatek_result nres = vatek_memfail;
	
	if (pnewdesc)
	{
		if (pepg->descriptors == NULL)pepg->descriptors = pnewdesc;
		else
		{
			Pmux_epg_desc plast = pepg->descriptors;
			while (plast->next)plast = plast->next;
			plast->next = pnewdesc;

			nres = vatek_success;
			*pdesc = pnewdesc;
		}
	}
	return nres;
}

vatek_result muxepg_check(Pmux_epg_param pepg)
{
	vatek_result nres = vatek_badparam;
	if (pepg->start_time.year != 0 || pepg->start_time.month != 0 || pepg->days != 0)
	{
		nres = vatek_success;
		if (pepg->start_time.year < 1900 || pepg->start_time.year > 3000)nres = vatek_badparam;
		else if (pepg->start_time.month < 1 || pepg->start_time.month > 12)nres = vatek_badparam;
		else if (pepg->start_time.day < 1 || pepg->start_time.day > 31)nres = vatek_badparam;
		else if (pepg->start_time.hour > 23)nres = vatek_badparam;
		else if (pepg->start_time.minute > 59)nres = vatek_badparam;
		else if (pepg->start_time.second > 59)nres = vatek_badparam;
	}
	if(!is_vatek_success(nres))VWAR("bad epg start_time [%4d/%2d/%2s %2d:%2d:%2d]",
									pepg->start_time.year,
									pepg->start_time.month,
									pepg->start_time.day,
									pepg->start_time.hour,
									pepg->start_time.minute,
									pepg->start_time.second);
	return nres;
}

Pmux_epg_section malloc_epg_section(uint8_t* pbuf, int32_t* pos)
{
	int32_t ptr = *pos;
	Pmux_epg_section psection = (Pmux_epg_section)&pbuf[ptr];
	ptr += sizeof(mux_epg_section);

	psection->title.text = &pbuf[ptr];
	psection->title.maxlen = EPG_TITLE_MAX_LEN;
	ptr += EPG_TITLE_MAX_LEN;

	psection->content.text = &pbuf[ptr];
	psection->content.maxlen = EPG_CONTENT_MAX_LEN;
	ptr += EPG_CONTENT_MAX_LEN;

	psection->duration = 1;
	psection->parental_rating = rating_general;
	psection->type = mux_content_news;

	psection->title.len = sizeof(EIT_EVENT_JIS_NAME3);
	memcpy(&psection->title.text[0], &EIT_EVENT_JIS_NAME3[0], psection->title.len);
	psection->content.len = sizeof(EIT_EVENT_JIS_NAME4);
	memcpy(&psection->content.text[0], &EIT_EVENT_JIS_NAME4[0], psection->content.len);

	*pos = ptr;
	return psection;
}
