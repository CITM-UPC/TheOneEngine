#ifndef __SCRIPTINGINPUT_H__
#define __SCRIPTINGINPUT_H__

class ScriptingInput {
public:
	ScriptingInput() {};
	~ScriptingInput() {};

	// Keyboard
	int		GetKeyState	(const char* key) const;
	bool	IsKeyDown	(const char* key) const;
	bool	IsKeyUp		(const char* key) const;
	bool	IsKeyRepeat	(const char* key) const;
	bool	IsKeyIdle	(const char* key) const;

	// Mouse
	int		GetMBState	(const char* mousebutton) const;
	bool	IsMBDown	(const char* mousebutton) const;
	bool	IsMBUp		(const char* mousebutton) const;
	bool	IsMBRepeat	(const char* mousebutton) const;
	bool	ISMBIdle	(const char* mousebutton) const;
	int		GetMouseMovementX() const;
	int		GetMouseMovementY() const;


};
#endif