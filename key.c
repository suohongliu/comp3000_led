#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KBD_IRQ             1       /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG        0x60    /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

#define SCROLL_LED 1
#define NUM_LED 2
#define CAPS_LED 4


void set_led(int,int,int);

unsigned char temp = 0;
void set_led(int numled,int capsled,int scrolled){
    //take argument to set temp;
	temp = (scrolled) ? (temp | SCROLL_LED) : (temp & SCROLL_LED);
	temp = (numled) ? (temp | NUM_LED) : (temp & NUM_LED);
	temp = (capsled) ? (temp | CAPS_LED) : (temp & CAPS_LED);
	
    //read keyboard status
    while((inb(0x64)&2)!=0){}
    //send command to port to set led
	outb(0xED,KBD_DATA_REG);
    //read again
	while((inb(0x64)&2)!=0){}
    //send value of led to keyboard
	outb(temp,KBD_DATA_REG);
}

static irqreturn_t kbd2_isr(int irq, void *dev_id)
{
    char scancode;

    //read keyboard status and set led
    scancode = inb(KBD_DATA_REG);

    pr_info("Scan Code %x %s\n",
            scancode & KBD_SCANCODE_MASK,
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");
	switch(scancode){
		case 0x01:	printk("You pressed Esc");
					set_led(0,0,0);
					break;
		case 0x3C:	printk("You pressed F2");
					set_led(0,1,0);
					break;
		case 0x3E:	printk("You pressed F4");
					set_led(1,1,1);
					break;
		default:
					break;
	}


    return IRQ_HANDLED;
}

static int __init kbd2_init(void)
{
    return request_irq(KBD_IRQ, kbd2_isr, IRQF_SHARED, "kbd2", (void *)kbd2_isr);
}

static void __exit kbd2_exit(void)
{
    free_irq(KBD_IRQ, (void *)kbd2_isr);
}

module_init(kbd2_init);
module_exit(kbd2_exit);

MODULE_LICENSE("GPL");
