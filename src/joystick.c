#include "include/joystick.h"
#include "include/utill.h"
#include <libudev.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#define ASCII_BTN_MIN ASCII_BTN_A
#define ASCII_BTN_MAX ASCII_BTN_RTHUMB
#define ASCII_AXIS_MIN ASCII_AXIS_LX
#define ASCII_AXIS_MAX ASCII_DPAD_Y
#define ASCII_AXIS_BITS ((1 << 15) - 1)
#define ASCII_TRIGGER_BITS ((1 << 8) - 1)
#define ASCII_DPAD_THRESH 10 
#define ASCII_DPAD_OFFSET 6
#define ASCII_DEADZONE 0.2
//static const char * buttonStrings[] = {"A", "B", NULL, "X", "Y", NULL, "LB", "RB", NULL, NULL, "SELECT", "START", "MODE", "THUMBL", "THUMBR"};
//static const char * axisStrings[] = {"LX", "LY", "LT", "RX", "RY", "RT", NULL, NULL, NULL, NULL, "DPAD X", "DPAD Y"};


struct  ascii_joystick_t{
	int fd;
	ASCIIBtnState buttons[15];
	ASCIIAxisState axis[12];	
	pthread_mutex_t mutex;
	pthread_t inputThread;
	int run;
};

static char *getJoyStickPath();

static void *ASCIIJoyStickUpdate(void* joystick_);

ASCIIJoyStick ASCIIJoyStickCreate(){
	ASCIIJoyStick joystick = malloc(sizeof(*joystick));
	if(!joystick)
		return NULL;
	char *path = getJoyStickPath();
	if(!path){
		free(joystick);
		return NULL;
	}
	joystick->fd = open(path, O_RDONLY | O_NONBLOCK);
	free(path);

	if(joystick->fd <= 0){
		free(joystick);
		return NULL;
	}

	for(int i = 0; i < 15;i++){
		joystick->buttons[i] = 0;
		joystick->axis[(int)minf(11, i)] = 0;
	}
	joystick->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	joystick->inputThread = 0;
	joystick->run = 0;
	return joystick;
}

//start the joystick
void ASCIIJoyStickStart(ASCIIJoyStick joystick){
	if(!joystick)
		return;
	joystick->run = 1;
	pthread_create(&(joystick->inputThread), NULL, ASCIIJoyStickUpdate, joystick);

}
//stop the joystick
void ASCIIJoyStickStop(ASCIIJoyStick joystick){
	if(!joystick)
		return;
	joystick->run = 0;
	pthread_join(joystick->inputThread, NULL);
}
void* ASCIIJoyStickUpdate(void* joystick_){
	if(!joystick_)
		return NULL;
	ASCIIJoyStick joystick = (ASCIIJoyStick)joystick_;
	struct input_event event;
	int r;
	while(joystick->run){
		r = read(joystick->fd, &event, 24);
		if(r <= 0)
			continue;
		if(event.type == EV_SYN)
			continue;
		pthread_mutex_lock(&joystick->mutex);

		if(event.type == EV_KEY){
			joystick->buttons[event.code - BTN_A] = event.value == 1 ? ASCII_BTN_PRESSED : ASCII_BTN_RELEASED;
		}

		if(event.type == EV_ABS){
			double normalized = 0;
			if(event.code > ASCII_DPAD_THRESH){
				event.code -= ASCII_DPAD_OFFSET;
				normalized = event.value ;
			}else{
				if(event.code == ABS_Z || event.code == ABS_RZ)
					normalized = event.value / (double)ASCII_TRIGGER_BITS;
				else{
					normalized = event.value / (double)ASCII_AXIS_BITS;
					if(fabs(normalized) < ASCII_DEADZONE)
						normalized = 0;
				}
				
			}

			joystick->axis[event.code - ABS_X] = normalized;
		}

		pthread_mutex_unlock(&joystick->mutex);
	}
	return NULL;
}

ASCIIBtnState ASCIIJoyStickGetButton(ASCIIJoyStick joystick, ASCIIBtn button){
	if(!joystick)
		return 0;
	if(button < ASCII_BTN_MIN  || button > ASCII_BTN_MAX)
		return 0;

	pthread_mutex_lock(&joystick->mutex);
	ASCIIBtnState res = joystick->buttons[button];
	pthread_mutex_unlock(&joystick->mutex);
	return res;
}

ASCIIAxisState ASCIIJoyStickGetAxis(ASCIIJoyStick joystick, ASCIIAxis axis){
	if(!joystick)
		return 0;
	if(axis < ASCII_AXIS_MIN  || axis > ASCII_AXIS_MAX)
		return 0;

	pthread_mutex_lock(&joystick->mutex);
	ASCIIAxisState res = joystick->axis[axis];
	pthread_mutex_unlock(&joystick->mutex);
	return res;
}

void ASCIIJoyStickDestroy(ASCIIJoyStick joystick){
	if(!joystick)
		return;
	close(joystick->fd);
	free(joystick);
}

static char *getJoyStickPath(){

	struct udev *context = udev_new();

	if(!context)
		return NULL;

	struct udev_monitor *monitor = udev_monitor_new_from_netlink(context, "udev");

	if(!monitor){
		udev_unref(context);
		return NULL;
	}

	udev_monitor_filter_add_match_subsystem_devtype(monitor, "input", NULL);
	udev_monitor_enable_receiving(monitor);

	struct udev_enumerate *enumerator = udev_enumerate_new(context);
	if(!enumerator){
		udev_monitor_unref(monitor);
		udev_unref(context);
		return NULL;
	}

	udev_enumerate_add_match_property(enumerator, "ID_INPUT_JOYSTICK", "1");
	udev_enumerate_scan_devices(enumerator);
	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerator);

	for(struct udev_list_entry* item = devices; item != NULL; item = udev_list_entry_get_next(item)) {
		const char *name = udev_list_entry_get_name(item);
		struct udev_device *device = udev_device_new_from_syspath(context, name);
		const char *deviceNode = udev_device_get_devnode(device);

		if(deviceNode){
			char *ret = malloc(strlen(deviceNode));
			memset(ret, 0, strlen(deviceNode));

			if(!ret){
				udev_enumerate_unref(enumerator);
				udev_monitor_unref(monitor);
				udev_unref(context);
				udev_device_unref(device);
				return NULL;
			}

			strcpy(ret, deviceNode);
			udev_enumerate_unref(enumerator);
			udev_monitor_unref(monitor);
			udev_unref(context);
			udev_device_unref(device);
			return ret;
		}

		udev_device_unref(device);
	}

	udev_enumerate_unref(enumerator);
	udev_monitor_unref(monitor);
	udev_unref(context);
	return NULL;
}



