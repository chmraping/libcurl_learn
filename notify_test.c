#include <stdio.h>
#include<libnotify/notify.h>
void main(void){
    NotifyNotification *p;
    notify_init("hello");
    p =notify_notification_new("hello", "good boy!", NULL);
    notify_notification_show(p, NULL);


}
