#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

#define MAJOR_NUMBER 240
#define DEVICE_NAME "ledirq_dd"
#define GPIO_DEBOUNCE_TIME 100
#define GPIO_OUT_PIN 171
#define GPIO_IN_PIN 174
#define LED_CLEAR 0
#define LED_SET 1

int gpio_irq_num = 0;
char toggle = 1;

int led_open(struct inode *, struct file *);
int led_release(struct inode *, struct file *);
long led_unlocked_ioctl(struct file *, unsigned int, unsigned long);
int led_init(void);
int led_exit(void);

int led_open(struct inode * inode_p, struct file * file_p){
	printk("[DEV] led_open() major : %d, minor : %d\n", MAJOR(inode_p->i_rdev), MINOR(inode_p->i_rdev));
	return 0;
}

int led_release(struct inode * inode_p, struct file * file_p){
	printk("[DEV] led_release()\n");
	return 0;
}

long led_unlocked_ioctl(struct file * file_p, unsigned int command, unsigned long argument){
	switch(command){
		case 0 : {
				 	if(argument == 0){
						gpio_set_value(GPIO_OUT_PIN, 0);
						printk("[DEV] led_ioctl() command : %d, argument : %d\n", command, (int)argument);
					}
					else if(argument == 1){
						gpio_set_value(GPIO_OUT_PIN, 1);
						printk("[DEV] led_ioctl() command : %d, argument : %d\n", command, (int)argument);
					}
				 }
				 break;
				 default:
				 	printk("[DEV] led_ioctl() error");
				 break;
	}
	return 0;
}

irqreturn_t irq_gpio(int irq, void * ident){
	printk("[DEV] interrupted by GPIO_174\n");
	if(toggle){
		toggle = !toggle;
		gpio_set_value(GPIO_OUT_PIN, 1);
		printk("[DEV] 1\n");
	}
	else{
		toggle = !toggle;
		gpio_set_value(GPIO_OUT_PIN, 0);
		printk("[DEV] 0\n");
	}
	return IRQ_HANDLED;
}

struct file_operations led_file_oper = {
	.owner = THIS_MODULE,
	.open = led_open,
	.unlocked_ioctl = led_unlocked_ioctl,
	.release = led_release,
};

int led_init(void){
	if(gpio_request(GPIO_OUT_PIN, "LED_OUT") != 0)
		printk("[DEV] request error\n");
	if(gpio_request(GPIO_IN_PIN, "LED_IN") != 0)
		printk("[DEV] request error\n");
	if(gpio_export(GPIO_OUT_PIN, 1) < 0)
		printk("[DEV] export error\n");
	if(gpio_export(GPIO_IN_PIN, 1) < 0)
		printk("[DEV] export error\n");
	if(gpio_direction_output(GPIO_OUT_PIN, 1) != 0)
		printk("[DEV] direction error\n");
	if(gpio_direction_input(GPIO_IN_PIN) != 0)
		printk("[DEV] direction error\n");
	if((register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &led_file_oper)) < 0){
		printk("[DEV] LED initialization is fail\n");
		return -1;
	}

	gpio_set_debounce(GPIO_IN_PIN, GPIO_DEBOUNCE_TIME);
	gpio_irq_num = gpio_to_irq(GPIO_IN_PIN);
	if(request_irq(gpio_irq_num, irq_gpio, IRQF_TRIGGER_RISING , "gpio_174", NULL) < 0)
		printk("[DEV] request irq error\n");
	irq_set_irq_type(gpio_irq_num, IRQ_TYPE_EDGE_RISING);
	printk("[DEV] LED initialization success\n");
	return 0;
}

int led_exit(void){
	printk("[DEV] LED module is closed\n");
	gpio_free(GPIO_OUT_PIN);
	gpio_free(GPIO_IN_PIN);
	free_irq(gpio_irq_num, NULL);
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
	return 0;
}

module_init(led_init);
module_exit(led_exit);
