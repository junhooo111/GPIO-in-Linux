#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>

MODULE_LICENSE("Dual BSD/GPL");

#define MAJOR_NUMBER 240
#define DEVICE_NAME "led_gp"
#define GPIO_OUT_PIN 171
#define LED_CLEAR 0
#define LED_SET 1

int led_open(struct inode *, struct file *);
int led_release(struct inode *, struct file *);
long led_unlocked_ioctl(struct file *,unsigned int, unsigned long);
int led_init(void);
int led_exit(void);

struct file_operations led_file_oper =
{
	.owner = THIS_MODULE,
	.open = led_open,
	.unlocked_ioctl = led_unlocked_ioctl,
	.release = led_release,
};

int led_open(struct inode * inode_p,struct file * file_p ){
	printk("[DEV]led_open\n");
	return 0;
}

int led_release(struct inode * inode_p,struct file * file_p){
	printk("[DEV] led_release\n");
	return 0;
}

long led_unlocked_ioctl(struct file * file_p, unsigned int command, unsigned long argument){
	switch(command){
		case 0:
		{
			if(argument == 0){
				gpio_set_value(GPIO_OUT_PIN,0);
				printk("[DEV] led_ioctl command : %d, argument : %d\n",command, (int)argument);
			}
			else if(argument ==1){
				gpio_set_value(GPIO_OUT_PIN,1);
				printk("[DEV] led_ioctl command : %d, argument : %d\n",command, (int)argument);
			}
		}
		break;
		default:
			printk("[DEV] led_ioctl() error");
		break;
	}
	return 0;
}

int led_init(void){
	if(gpio_request(GPIO_OUT_PIN,"LED_TEST")!=0)
		printk("[DEV]requeset error\n");

	if(gpio_export(GPIO_OUT_PIN,1)<0)
		printk("[DEV]export error\n");

	if(gpio_direction_output(GPIO_OUT_PIN,1)!=0)
		printk("[DEV]direction error\n");

	if((register_chrdev(MAJOR_NUMBER,DEVICE_NAME,&led_file_oper))<0){
		printk("[DEV]LED initialization is fail\n");
		return -1;
	}

	printk("[DEV]LED initialization is success\n");
	return 0;
}

int led_exit(void){
	printk("[DEV]LED module is closed\n");
	gpio_free(GPIO_OUT_PIN);
	unregister_chrdev(MAJOR_NUMBER,DEVICE_NAME);
	return 0;
}

module_init(led_init);
module_exit(led_exit);
