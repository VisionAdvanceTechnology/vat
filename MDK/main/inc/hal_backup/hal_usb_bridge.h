#ifndef _HAL_USB_BRIDGE_
#define _HAL_USB_BRIDGE_

#include <stdint.h>

typedef void* hbridge_device;
typedef void* hbridge_list;

#define USB_BRIDGE_VID				0x1934
#define USB_BRUDGE_PID				0x0122

#define USB_CMD_TAG					0x80000000
#define IS_USB_CMD(a)				((a & USB_CMD_TAG) == USB_CMD_TAG)
	
#define USB_CMD_LOCK				(USB_CMD_TAG | 0x1000)
#define USB_CMD_UNLOCK				(USB_CMD_LOCK + 1)
#define USB_CMD_GETSTATUS			(USB_CMD_UNLOCK + 1)

typedef enum _bridge_device_status
{
    hid_status_idle = 100,
    hid_status_waitlock = 101,
    hid_status_locked = 102,
    hid_status_waitunlock = 103,
}bridge_device_status;

static const char* usb_hid_status_str[] = 
{
    "idle","wait","locked","broadcast","unlocked",
};

#define is_current_usb_hid_status(status)  (status >= hid_status_idle && status <= hid_status_waitunlock)

static const char* usb_hid_status_get_str(bridge_device_status status)
{
    if(is_current_usb_hid_status(status))return usb_hid_status_str[status - hid_status_idle];
    return "unknown";
}

/* modulator chip command */
#define MODULATOR_CMD_TAG       0x02000000
#define IS_MODULATOR_CMD(a)     ((a & MODULATOR_CMD_TAG) == MODULATOR_CMD_TAG)

#define MODULATOR_CMD_OPEN      MODULATOR_CMD_TAG
#define MODULATOR_CMD_CLOSE     (MODULATOR_CMD_OPEN + 1)
#define MODULATOR_CMD_TRANSFER  (MODULATOR_CMD_CLOSE + 1)
#define MODULATOR_CMD_GETNUMS   (MODULATOR_CMD_TRANSFER + 1)

/* board bridge command (used base param)*/
#define BOARDBRIDGE_CMD_TAG		0x04000000
#define IS_BOARDBRIDGE_CMD(a)	((a & BOARDBRIDGE_CMD_TAG) == BOARDBRIDGE_CMD_TAG)


const static char hid_bridge_tag[4] =
{
    'v','a','-','1',
};

typedef struct _bridge_base_param
{
    uint32_t params[13];
}bridge_base_param, *Pbridge_base_param;

#define BRIDGE_PARAM_MAX_LEN	52


/* each param cost 52 bytes */
#define TURNKEY_I2C_MAX_BUFFER_LEN 36
typedef struct _bridge_i2c_param
{
    uint32_t handle;
    uint32_t address;
    uint32_t len;
    uint8_t buffer[TURNKEY_I2C_MAX_BUFFER_LEN];
}bridge_i2c_param, *Pbridge_i2c_param;


#define BRIDGE_MOD_BUFFER_LEN   32

#define FLAG_MOD_REGISTER       0x00000100
#define FLAG_MOD_WRITE          0x00000001
#define FLAG_MOD_CODE           0x01000000
#define FLAG_MOD_BRIDGE			0x00010000

#define MOD_RD_REG              (FLAG_MOD_REGISTER)
#define MOD_WR_REG              (FLAG_MOD_REGISTER | FLAG_MOD_WRITE)
#define MOD_RD_MEM              0
#define MOD_WR_MEM              FLAG_MOD_WRITE
#define MOD_WR_CODE             (FLAG_MOD_WRITE | FLAG_MOD_CODE)
#define MOD_RD_CODE             (FLAG_MOD_CODE)

#define MOD_DATA_IS_WRITE(a)    ((a & FLAG_MOD_WRITE) == FLAG_MOD_WRITE)
#define MOD_DATA_IS_REG(a)      ((a & FLAG_MOD_REGISTER) == FLAG_MOD_REGISTER)
#define MOD_DATA_IS_CODE(a)     ((a & FLAG_MOD_CODE) == FLAG_MOD_CODE)

typedef struct _bridge_mod_param
{
    uint32_t address;
    uint32_t len;
    int32_t index;
    uint32_t datatype;
    uint8_t buffer[BRIDGE_MOD_BUFFER_LEN];
}bridge_mod_param,*Pbridge_mod_param;

typedef struct _hid_bridge_cmd
{
    uint8_t tag[4];
    uint32_t cmd;
    uint32_t recv;
    union _bridge_cmd_param
    {
        uint8_t raw[BRIDGE_PARAM_MAX_LEN];
        bridge_base_param base;
        bridge_i2c_param i2c;
        bridge_mod_param mod;
    }param;
}hid_bridge_cmd, *Phid_bridge_cmd;

#define HALBRIDGE_ERR_NODEVICE		-9
#define HALBRIDGE_ERR_BUSY			-8
#define HALBRIDGE_ERR_TIMEOUT		-7
#define HALBRIDGE_ERR_HWFAIL		-6
#define HALBRIDGE_ERR_BADSTATUS		-5
#define HALBRIDGE_ERR_BUFOVERFLOW	-4
#define HALBRIDGE_ERR_BADPARAM		-3
#define HALBRIDGE_ERR_UNSUPPORT		-2
#define HALBRIDGE_ERR_UNKNOWN		-1
#define HALBRIDGE_ERR_SUCCESS		0

typedef struct _hid_bridge_result
{
    uint8_t tag[4];
    uint32_t cmd;
    int32_t result;
    union _bridge_result_data
    {
        uint8_t raw[BRIDGE_PARAM_MAX_LEN];
        bridge_i2c_param i2c;
        bridge_mod_param mod;
    }data;
}hid_bridge_result, *Phid_bridge_result;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif

