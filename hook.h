#include <iostream>
#include <Windows.h>
#include <string.h>
#include <map>
//#include <sys/time.h>

//#define _WIN32_WINNT 0x050
using namespace std;


struct DeviceKey{
    int button;
	int toCode;
	char device[255];

	DeviceKey(){}

    DeviceKey(char * device, int button, int toCode){
		strcpy(this->device,device);
        this->button = button;
        this->toCode = toCode;

    }
};





DeviceKey events[10];
int EVENTS_SUM = 0;


int getConst(char * constante){
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
			int button = atoi(pch);

			pch = strtok (NULL, " \t");
			char * toCode = pch;

			printf("%s %d %s\n", device, button, toCode);
			
			events[EVENTS_SUM] = DeviceKey(device, button , getConst(toCode) );
			EVENTS_SUM++;

			
		}
		fclose(file);
	}

}



/*long int getTimestamp(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}*/



/*

#define EVENTS_LENGTH 2
CodeEvt events[EVENTS_LENGTH];

void setEvents(){
    events[0] = CodeEvt(WM_LBUTTONDOWN,'A');
    events[1] = CodeEvt(WM_RBUTTONDOWN,'D');
}

/*
CodeEvt getEvent(WPARAM wParam){
    if (wParam == -1){
        return CodeEvt();
    }

    for (int i = 0; i < EVENTS_LENGTH; i++){
        if (wParam == events[i].code){
            events[i].up = false;
            return events[i];
        } else
        if (wParam == events[i].upCode){
            events[i].up = true;
            return events[i];
        }
    }
    
    return CodeEvt();
}*/

void VRPN_CALLBACK handle_button(void *userdata, const vrpn_BUTTONCB b) {
    const char *name = (const char *)userdata;

	for (int i = 0; i < EVENTS_SUM; i++){
		printf("%s == %s -> %d\n",name, events[i].device, strcmp(name,events[i].device));

		printf("%d == %d\n",b.button, events[i].button);


		if (strcmp(name,events[i].device) == 0 && b.button == events[i].button){
			printf("ACIONA %d" , events[i].toCode);
			keybd_event(events[i].toCode, 0, 0, 0);
			keybd_event(events[i].toCode, 0, KEYEVENTF_KEYUP, 0);
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


void call_evt(BYTE keyCode){
	keybd_event(keyCode, 0, 0, 0);
	keybd_event(keyCode, 0, KEYEVENTF_KEYUP, 0);
}

/*
LRESULT CALLBACK keyToKey(int nCode, WPARAM wParam, LPARAM lParam) {
    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION) {

        //Se foi um evento do teclado
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP){
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

            CodeEvt evt = getEvent(p->vkCode);
            if (evt.code != -1){
                fEatKeystroke = TRUE;//cancela a tecla real

                cout << "KEY TO KEY:"<< evt.toCode << "\n";
                if (wParam == WM_KEYDOWN){
                    keybd_event(evt.toCode, 0, 0, 0);
                } else
                if (wParam == WM_KEYUP){
                    keybd_event(evt.toCode, 0, KEYEVENTF_KEYUP, 0);
                }
            }

        } else {


            //Se foi um evento do mouse
            CodeEvt evt =  CodeEvt();
            evt.code = -1;

            if(wParam == WM_MOUSEMOVE){

                MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
                //cout << "x:" << pMouseStruct->pt.x << "y" << pMouseStruct->pt.y << "\n";

                int movement = -1;

                //cout << pMouseStruct->pt.y << " < " << screenHeight / 3 << "\n";
                if (pMouseStruct->pt.y < screenHeight / 3){
                    if (MOUSE_VERTICAL_RELEASE == -1){
                        movement = MOUSE_MOVE_UP_PRESS;
                        MOUSE_VERTICAL_RELEASE = MOUSE_MOVE_UP_RELEASE;
                    }
                } else
                if (pMouseStruct->pt.y > (screenHeight / 3)*2){
                    if (MOUSE_VERTICAL_RELEASE == -1){
                        movement = MOUSE_MOVE_DOWN_PRESS;
                        MOUSE_VERTICAL_RELEASE = MOUSE_MOVE_DOWN_RELEASE;
                    }
                } else
                if (MOUSE_VERTICAL_RELEASE != -1){
                    movement = MOUSE_VERTICAL_RELEASE;
                    MOUSE_VERTICAL_RELEASE = -1;
                }

                if (pMouseStruct->pt.x < screenWidth / 3){
                    if (MOUSE_HORIZONTAL_RELEASE == -1){
                        movement = MOUSE_MOVE_LEFT_PRESS;
                        MOUSE_HORIZONTAL_RELEASE = MOUSE_MOVE_LEFT_RELEASE;
                    }
                } else
                if (pMouseStruct->pt.x > (screenWidth / 3)*2){
                    if (MOUSE_HORIZONTAL_RELEASE == -1){
                        movement = MOUSE_MOVE_RIGHT_PRESS;
                        MOUSE_HORIZONTAL_RELEASE = MOUSE_MOVE_RIGHT_RELEASE;
                    }
                } else
                if (MOUSE_HORIZONTAL_RELEASE != -1 && movement != MOUSE_MOVE_DOWN_RELEASE && movement != MOUSE_MOVE_UP_RELEASE){
                    movement = MOUSE_HORIZONTAL_RELEASE;
                    MOUSE_HORIZONTAL_RELEASE = -1;
                }


                //Se foi um evento do mouse
                evt = getEvent(movement);

            } else {
                //Se foi um evento do mouse
                evt = getEvent(wParam);
            }

            if (evt.code != -1){//Se a tecla estiver configurada

                //fEatKeystroke = TRUE;//cancela a tecla real
                cout << "MOUSE TO KEY:"<< evt.toCode << "\n";
                if (evt.up){//Solta a tecla
                    cout << "keypressup1\n";
                    keybd_event(evt.toCode, 0, KEYEVENTF_KEYUP, 0);
                } else {//pressiona a tecla
                    keybd_event(evt.toCode, 0, 0, 0);
                    cout << evt.downAndUp << "keypress\n";
                    if (evt.downAndUp){//Alguns eventos do mouse so sao executados uma vez
                        cout << "keypressup2\n";
                        keybd_event(evt.toCode, 0, KEYEVENTF_KEYUP, 0);
                    }
                }
            }
        }

    }

    return(fEatKeystroke ? 1 : CallNextHookEx(hMouse, nCode, wParam, lParam));
}
*/
/*

void getScreenSize(){
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    screenHeight = desktop.bottom;
    screenWidth = desktop.right;
}

/*
int main() {
    // Install the low-level keyboard & mouse hooks
//    HWND windowHandle = FindWindow(NULL, _T("Impossible Game.exe"));
    //DWORD threadId = GetWindowThreadProcessId(windowHandle, NULL);

    getScreenSize();



    setEvents();

    hKybd  = SetWindowsHookEx(WH_KEYBOARD_LL, keyToKey, 0, 0);
    hMouse = SetWindowsHookEx(WH_MOUSE_LL, keyToKey, 0, 0);

    // Keep this app running until we're told to stop
    MSG msg;
    //this while loop keeps the hook
    while (!GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKybd);
    UnhookWindowsHookEx(hMouse);

    return(0);
}*/
