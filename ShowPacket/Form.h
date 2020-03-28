
BOOL CALLBACK Dlgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD ShowDialog(HMODULE hInstance);
void DisplayPacket(char *direction, char *buf, int len);
void CreatePacketString(BYTE *BufferPointer, DWORD BufferLength, char *PacketString);
void FormatMyString(char *src, char *dst);
DWORD StringToByteArray(char *str, char *pbyte);
void SendPacketInEdit(HWND Edit);
void SendRecvInEdit(HWND Edit);
void ClearListBox(HWND ListBox);
void SelectedPacketsToClipboard(HWND ListBox);
void AllPacketsToClipboard(HWND ListBox);
void CopyToClipboard(char *source);
void CatchKeystrokes(HWND hWnd);
void TryToLogin(char *user);
void LoginSuccessful();
void StartBot();
void StopBot();



void AddExIncludePacket(HWND ListBox);