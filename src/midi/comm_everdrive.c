#include "comm_everdrive.h"

#define SSF_REG16(reg)  (0xA13000 + reg)
#define REG_USB         0xE2
#define REG_STE         0xE4
#define STE_USB_RD_RDY  4
#define STE_USB_WR_RDY  2
#define STE_SPI_RDY     1

#define IO_STATUS_HI_SD    0x00
#define IO_STATUS_HI_SDHC  0x40

static bool is_present = false;

void comm_everdrive_init(void)
{
    u8 status = *(volatile u16*)SSF_REG16(REG_STE) >> 8;
    is_present = (status == IO_STATUS_HI_SD || status == IO_STATUS_HI_SDHC);
}

bool comm_everdrive_is_present(void)
{
    return is_present;
}

u8 comm_everdrive_read_ready(void)
{
    return *(volatile u16*)SSF_REG16(REG_STE) & STE_USB_RD_RDY;
}

u8 comm_everdrive_read(void)
{
    return *(volatile u16*)SSF_REG16(REG_USB);
}

u8 comm_everdrive_write_ready(void)
{
    return *(volatile u16*)SSF_REG16(REG_STE) & STE_USB_WR_RDY;
}

void comm_everdrive_write(const u8* data, u16 length)
{
    for (u16 i = 0; i < length; i++) {
        while (!comm_everdrive_write_ready());
        *(volatile u16*)SSF_REG16(REG_USB) = data[i];
    }
}
