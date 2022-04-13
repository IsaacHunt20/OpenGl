#pragma once

class IndexBuffer
{

private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	
	~IndexBuffer();
	
	void Bind() const;
	void UnBind() const;
	/*
	TODO Lock/Unlcok data to render more complicated stuff 
	*/

	inline unsigned int GetCount() const { return m_Count; }
};

