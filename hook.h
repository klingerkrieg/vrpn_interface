#include <iostream>
#include <Windows.h>
#include <string.h>


using namespace std;

int scanCode[] = {' ',VK_ESCAPE,'1','2','3','4','5','6','7','8','9','0',
'-','=',VK_BACK,VK_TAB,'Q','W','E','R','T','Y','U','I','O','P','[',']',
VK_RETURN,VK_LCONTROL,'A','S','D','F','G','H','J','K','L',
';','"','~',VK_LSHIFT,'\\','Z','X','C','V','B','N','M',',','.'};


struct DeviceKey{
    int button;
	int toCode;
	char device[255];
	bool pressed;

	DeviceKey(){}

    DeviceKey(char * device, int button, int toCode){
		strcpy(this->device,device);
        this->button = button;
        this->toCode = toCode;
		this->pressed = false;

    }
};





DeviceKey events[10];
int EVENTS_SUM = 0;


int getConst(char * constante){
	//Caso seja o numero do botao do mouse 1 2 ou 3
	if (strcmp(constante,"LEFT") == 0) {
		return 0;
	} else
	if (strcmp(constante,"MIDDLE") == 0) {
		return 1;
	} else
	if (strcmp(constante,"RIGHT") == 0) {
		return 2;
	} else
	if (strcmp(constante,"WM_LBUTTONDOWN") == 0){
		return WM_LBUTTONDOWN;
	} else 
	if (strcmp(constante,"WM_RBUTTONDOWN") == 0){
		return WM_RBUTTONDOWN;
	} else {
		//Caso seja uma letra retorna o char code
		if (strlen(constante) <= 2){
			return constante[0];
		}
		return 0;
	}
}

void readConfFile(){

	char str[512];
	FILE * file;
	file = fopen( "hook.cfg" , "r");
	
	
	if (file) {
		while ( fgets (str , 100 , file) != NULL ){
			//Caso seja comentario
			if (str[0] == '#'){
				continue;
			}

			char * pch = strtok (str,"\t");
			char * device = pch;
			
			pch = strtok (NULL, " \t");
			char * from = pch;

			pch = strtok (NULL, " \t");
			char * toCode = pch;

			printf("%s %d %s\n", device, from, toCode);
			
			events[EVENTS_SUM] = DeviceKey(device, getConst(from) , getConst(toCode) );
			EVENTS_SUM++;

			
		}
		fclose(file);
	}

}

bool isKeyBoard(const char * name){
	char kbd[] = "Keyboard";
	for (int i = 0; i < 8; i++){
		if (name[i] != kbd[i]){
			return false;
		}
	}
	return true;
}


void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
    const char *name = (const char *)userdata;



	//Percorre todos os botoes registrados
	for (int i = 0; i < EVENTS_SUM; i++){
		//printf("%s == %s -> %d\n",name, events[i].device, strcmp(name,events[i].device));
		printf("%d == %d\n", b.button, events[i].button);

		bool kbd = isKeyBoard(name);c

		//Caso o botao pressionado esteja registrado
		if (strcmp(name,events[i].device) == 0 &&
				( kbd && scanCode[b.button] == events[i].button ||
				  kbd == false && b.button == events[i].button) ){
			//Caso seja keyboard ele ira converter do scancode para ascii
			//printf("state %d pressed %d", b.state, events[i].pressed);
			//Caso nao seja usa o proprio codigo

			//Se for evento para pressionar
			if (events[i].pressed == false && b.state == 1){
				printf("ACIONA %d" , events[i].toCode);
				keybd_event(events[i].toCode, 0, 0, 0);
				events[i].pressed = true;
			} else //Se estiver soltando o botao
			if (events[i].pressed == true && b.state == 0){
				printf("SOLTA %d" , events[i].toCode);
				keybd_event(events[i].toCode, 0, KEYEVENTF_KEYUP, 0);
				events[i].pressed = false;
			}

			
		}
	}

    printf("##########################################\r\n"
           "Button %s, number %d was just %s\n !!!"
           "##########################################\r\n",
           name, b.button, b.state ? "pressed" : "released");
}

void VRPN_CALLBACK
handle_button_states(void *userdata, const vrpn_BUTTONSTATESCB b)
{
    const char *name = (const char *)userdata;

    printf("Button %s has %d buttons with states!!!:", name, b.num_buttons);
    int i;
    for (i = 0; i < b.num_buttons; i++) {
        printf(" %d", b.states[i]);
    }
    printf("\n");
}


void VRPN_CALLBACK handle_analog(void *userdata, const vrpn_ANALOGCB a)
{
    int i;
    const char *name = (const char *)userdata;

    printf("!!!Analog %s:\n         %5.2f", name, a.channel[0]);
    for (i = 1; i < a.num_channel; i++) {
        printf(", %5.2f", a.channel[i]);
    }
    printf(" (%d chans)\n", a.num_channel);
}

