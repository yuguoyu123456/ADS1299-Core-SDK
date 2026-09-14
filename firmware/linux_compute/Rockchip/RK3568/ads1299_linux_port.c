#define _POSIX_C_SOURCE 200809L
#include "ads1299_linux_port.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/gpio.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

static void fds_init(rk3568_ads1299_linux_t *c){c->spi_fd=c->cs_fd=c->reset_fd=c->pwdn_fd=c->start_fd=c->drdy_fd=-1;}
static int req_line(int chip,unsigned off,unsigned long flags,int value,const char *name){
 struct gpiohandle_request r; memset(&r,0,sizeof(r)); r.lineoffsets[0]=off; r.flags=flags; r.lines=1; r.default_values[0]=value?1u:0u; strncpy(r.consumer_label,name,sizeof(r.consumer_label)-1u); return ioctl(chip,GPIO_GET_LINEHANDLE_IOCTL,&r)<0?-errno:r.fd;
}
static int req_optional(int chip,unsigned off,unsigned long flags,int value,const char *name){return off==UINT_MAX?-1:req_line(chip,off,flags,value,name);}
static int line_set(int fd,int value){struct gpiohandle_data d; if(fd<0)return -ENODEV; memset(&d,0,sizeof(d)); d.values[0]=value?1u:0u; return ioctl(fd,GPIOHANDLE_SET_LINE_VALUES_IOCTL,&d)<0?-errno:0;}
static int line_get(int fd){struct gpiohandle_data d; if(fd<0)return -ENODEV; memset(&d,0,sizeof(d)); if(ioctl(fd,GPIOHANDLE_GET_LINE_VALUES_IOCTL,&d)<0)return -errno; return d.values[0]?1:0;}
static int spi_cb(void *u,const uint8_t *tx,uint8_t *rx,size_t n){rk3568_ads1299_linux_t *c=u; struct spi_ioc_transfer t; if(!c||c->spi_fd<0||!n)return -EINVAL; memset(&t,0,sizeof(t)); t.tx_buf=(uintptr_t)tx; t.rx_buf=(uintptr_t)rx; t.len=(uint32_t)n; t.speed_hz=c->spi_hz; t.bits_per_word=8u; return ioctl(c->spi_fd,SPI_IOC_MESSAGE(1),&t)<0?-errno:0;}
static void cs_cb(void*u,int v){rk3568_ads1299_linux_t*c=u;if(c)(void)line_set(c->cs_fd,v);} static void rst_cb(void*u,int v){rk3568_ads1299_linux_t*c=u;if(c&&c->reset_fd>=0)(void)line_set(c->reset_fd,v);} static void pwdn_cb(void*u,int v){rk3568_ads1299_linux_t*c=u;if(c&&c->pwdn_fd>=0)(void)line_set(c->pwdn_fd,v);} static void start_cb(void*u,int v){rk3568_ads1299_linux_t*c=u;if(c&&c->start_fd>=0)(void)line_set(c->start_fd,v);} static int drdy_cb(void*u){rk3568_ads1299_linux_t*c=u;return c?line_get(c->drdy_fd):-EINVAL;}
static void delay_cb(void*u,uint32_t us){(void)u;struct timespec r={(time_t)(us/1000000u),(long)((us%1000000u)*1000u)};while(nanosleep(&r,&r)<0&&errno==EINTR){}}

int rk3568_ads1299_linux_open(rk3568_ads1299_linux_t*c,const rk3568_ads1299_linux_config_t*cfg){
 int chip; uint8_t mode=(uint8_t)(SPI_MODE_1|SPI_NO_CS),bits=8u; uint32_t speed; if(!c||!cfg||!cfg->spidev_path||!cfg->gpiochip_path||!cfg->spi_hz||cfg->cs_line==UINT_MAX)return -EINVAL; fds_init(c); c->spi_hz=cfg->spi_hz; c->spi_fd=open(cfg->spidev_path,O_RDWR|O_CLOEXEC); if(c->spi_fd<0)return -errno; speed=cfg->spi_hz; if(ioctl(c->spi_fd,SPI_IOC_WR_MODE,&mode)<0||ioctl(c->spi_fd,SPI_IOC_WR_BITS_PER_WORD,&bits)<0||ioctl(c->spi_fd,SPI_IOC_WR_MAX_SPEED_HZ,&speed)<0){int e=errno;rk3568_ads1299_linux_close(c);return -e;} chip=open(cfg->gpiochip_path,O_RDONLY|O_CLOEXEC); if(chip<0){int e=errno;rk3568_ads1299_linux_close(c);return -e;} c->cs_fd=req_line(chip,cfg->cs_line,GPIOHANDLE_REQUEST_OUTPUT,1,"ads1299-cs"); c->reset_fd=req_optional(chip,cfg->reset_line,GPIOHANDLE_REQUEST_OUTPUT,1,"ads1299-reset"); c->pwdn_fd=req_optional(chip,cfg->pwdn_line,GPIOHANDLE_REQUEST_OUTPUT,1,"ads1299-pwdn"); c->start_fd=req_optional(chip,cfg->start_line,GPIOHANDLE_REQUEST_OUTPUT,0,"ads1299-start"); c->drdy_fd=req_optional(chip,cfg->drdy_line,GPIOHANDLE_REQUEST_INPUT,0,"ads1299-drdy"); close(chip); if(c->cs_fd<0||(cfg->reset_line!=UINT_MAX&&c->reset_fd<0)||(cfg->pwdn_line!=UINT_MAX&&c->pwdn_fd<0)||(cfg->start_line!=UINT_MAX&&c->start_fd<0)||(cfg->drdy_line!=UINT_MAX&&c->drdy_fd<0)){rk3568_ads1299_linux_close(c);return -EIO;} return 0;
}
void rk3568_ads1299_linux_close(rk3568_ads1299_linux_t*c){if(!c)return; if(c->drdy_fd>=0)close(c->drdy_fd);if(c->start_fd>=0)close(c->start_fd);if(c->pwdn_fd>=0)close(c->pwdn_fd);if(c->reset_fd>=0)close(c->reset_fd);if(c->cs_fd>=0)close(c->cs_fd);if(c->spi_fd>=0)close(c->spi_fd);fds_init(c);c->spi_hz=0u;}
int rk3568_ads1299_linux_bind(rk3568_ads1299_linux_t*c,ads1299_port_t*p){if(!c||!p||c->spi_fd<0||c->cs_fd<0)return -EINVAL;memset(p,0,sizeof(*p));p->user=c;p->spi_transfer=spi_cb;p->cs_write=cs_cb;p->reset_write=c->reset_fd>=0?rst_cb:NULL;p->pwdn_write=c->pwdn_fd>=0?pwdn_cb:NULL;p->start_write=c->start_fd>=0?start_cb:NULL;p->drdy_read=c->drdy_fd>=0?drdy_cb:NULL;p->delay_us=delay_cb;return 0;}
