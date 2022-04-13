#pragma once

class VertexBuffer
{

private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	
	~VertexBuffer();
	
	void Bind() const;
	void UnBind() const;
	/*
	TODO Lock/Unlcok data to render more complicated stuff 
	*/
};

