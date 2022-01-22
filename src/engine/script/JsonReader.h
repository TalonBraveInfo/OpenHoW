// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

class JsonReader {
public:
	explicit JsonReader( const std::string &path );
	JsonReader();
	~JsonReader();

	void ParseBuffer( const char *buf );

	bool EnterChildNode( const std::string &property, bool silent = true );
	void EnterChildNode( unsigned int index );
	void LeaveChildNode();

	std::string GetStringProperty( const std::string &property, const std::string &def = "", bool silent = true );
	int GetIntegerProperty( const std::string &property, int def = 0, bool silent = true );
	bool GetBooleanProperty( const std::string &property, bool def = false, bool silent = true );
	float GetFloatProperty( const std::string &property, float def = 0.0f, bool silent = true );
	hei::Colour GetColourProperty( const std::string &property, const hei::Colour &def = { 0, 0, 0 }, bool silent = true );
	PLVector3 GetVector3Property( const std::string &property, PLVector3 def = { 0, 0, 0 }, bool silent = true );
	PLVector4 GetVector4Property( const std::string &property, PLVector4 def = { 0, 0, 0, 0 }, bool silent = true );

	std::list<std::string> GetObjectKeys();

	unsigned int GetArrayLength( const std::string &property = "", bool silent = true );
	std::vector<std::string> GetArrayStrings( const std::string &property, bool silent = true );
	std::string GetArrayStringProperty( const std::string &property, unsigned int index );

protected:
private:
	void *ctx_{ nullptr };
};
