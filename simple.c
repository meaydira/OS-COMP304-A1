//
//  p4.c
//  assignment1
//
//  Created by Dilara on 17.02.2020.
//

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>


int simple init(void)
{
printk(KERN INFO "Loading Module\n");
return 0;
}

void simple exit(void)
{
printk(KERN INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module init(simple init);
module exit(simple exit);
MODULE LICENSE("GPL");
MODULE DESCRIPTION("Simple Module");
MODULE AUTHOR("SGG");
