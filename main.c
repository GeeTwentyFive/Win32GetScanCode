// gcc <>.c -Wl,-e_start -nostdlib -O3 -march=x86-64-v3 -fno-pie -no-pie -pipe C:\Windows\System32\kernel32.dll C:\Windows\System32\user32.dll



static void* GLOBAL_std_out_handle;

__declspec(dllimport) __fastcall void* GetStdHandle(unsigned long);
static inline void Init() {

	GLOBAL_std_out_handle = GetStdHandle(-11);

}



__declspec(dllimport) __fastcall int WriteConsoleW(void*, unsigned short*, unsigned long, unsigned long*, void*);
static inline void WriteOut(unsigned short* out, long out_len) {

	WriteConsoleW(GLOBAL_std_out_handle, out, out_len, 0, 0);

}

// For string constants
#define WRITE_OUT(x) WriteOut(L##x "\n", sizeof(L##x "\n")/2 - 1);

__declspec(dllimport) __fastcall int wsprintfW(unsigned short*, unsigned short*, ...);
static inline void WriteOutNum(long long num) {

	static unsigned short out[32];
	WriteOut(out, wsprintfW(out, L"%d\n", num));

}



__declspec(dllimport) __fastcall void* DefWindowProcW(void*, unsigned int, void*, void*);
__declspec(dllimport) __fastcall void* GetModuleHandleW(unsigned short*);
__declspec(dllimport) __fastcall unsigned short RegisterClassExW(void*); // WNDCLASSEX*
__declspec(dllimport) __fastcall void* CreateWindowExW(unsigned long, unsigned short*, unsigned short*, unsigned long, int, int, int, int, void*, void*, void*, void*);
__declspec(dllimport) __fastcall int RegisterRawInputDevices(void*, unsigned int, unsigned int); // RAWINPUTDEVICE[]*
__declspec(dllimport) __fastcall int GetMessageW(void*, void*, unsigned int, unsigned int); // MSG*
__declspec(dllimport) __fastcall unsigned int GetRawInputData(void*, unsigned int, void*, unsigned int*, unsigned int); // HRAWINPUT*, RAWINPUT*
__declspec(dllimport) __fastcall void ExitProcess(unsigned int);
void _start() {

	Init();



	static unsigned short CLASS_NAME[] = L"main_wc";

	static struct {

		unsigned int size;

		unsigned int _1;

		void* (*WndProc)(void*, unsigned int, void*, void*);

		unsigned int _2[2];

		void* h_instance;

		void* _3[4];

		unsigned short* class_name;

		void* _4;

	} wc = {

		.size = sizeof(wc),
		.class_name = CLASS_NAME

	};
	wc.WndProc = DefWindowProcW;
	wc.h_instance = GetModuleHandleW(0);

	if (RegisterClassExW(&wc) == 0) { WRITE_OUT("ERROR: RegisterClassExW() failed"); ExitProcess(1); }

	static void* hwnd;
	hwnd = CreateWindowExW(0, CLASS_NAME, 0, 0, 0, 0, 0, 0, (void*)-3, 0, wc.h_instance, 0);
	if (hwnd == (void*)0) { WRITE_OUT("ERROR: CreateWindowExW() failed"); ExitProcess(1); }

	static struct {

		unsigned short usage_page;
		unsigned short usage;
		unsigned long flags;
		void* hwnd_target;

	} rid = {1, 6, 0x00000130};
	rid.hwnd_target = hwnd;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == 0) { WRITE_OUT("ERROR: RegisterRawInputDevices() failed"); ExitProcess(1); }



	WRITE_OUT("Running...");

	static struct {

		void* _hwnd;

		unsigned int message;

		void* _w_param;

		void* l_param;

		unsigned long _rest[4];

	} msg;
	while (GetMessageW(&msg, 0, 0xFF, 0xFF)) {

		static struct {

			unsigned char _raw_input_header[24];

			unsigned short scan_code;
			unsigned short flags;

			unsigned char _rest[12];

		} kb_data;
		static unsigned int kb_data_size = sizeof(kb_data);
		switch (GetRawInputData(msg.l_param, 0x10000003, &kb_data, &kb_data_size, 24)) {

			case -1:
				WRITE_OUT("ERROR: GetRawInputData() failed");
				ExitProcess(1);

			case 0: break;

			default:

				if (kb_data.flags == 0) { // Key down

					WriteOutNum(kb_data.scan_code);

				}

		}

	}



	ExitProcess(0);

}
