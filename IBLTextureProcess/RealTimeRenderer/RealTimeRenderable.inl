

template<typename ObjectType>
RealTimeRenderable<ObjectType>::~RealTimeRenderable()
{
	glDeleteBuffers(1, &PosBuffer);
	glDeleteBuffers(1, &UVBuffer);
	glDeleteBuffers(1, &NormalBuffer);
	glDeleteBuffers(1, &LightMapBuffer);
	glDeleteBuffers(1, &IndexBuffer);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &TextureName);

}


template<typename ObjectType>
void RealTimeRenderable<ObjectType>::Draw(RealTimeRenderer<ObjectType> *RTRenderer)
{
	if (Program != nullptr)
	{
		PBRGLSL* PBR = dynamic_cast<PBRGLSL*>(Program);
		if (PBR)
		{
			PBR->Draw(this, RTRenderer);
			return;
		}
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glUniform1i(RTRenderer->TextureLoc, 0);
	RTRenderer->SetModelMatrix(Object->GetModelMatrix());
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, Object->GetPosData().size() * 3);
}

template<typename ObjectType>
void RealTimeRenderable<ObjectType>::InitRealTimeRenderable(const ObjectType *InObject, RealTimeRenderer<ObjectType> *RTRenderer)
{
	if (InObject == nullptr) return;

	Object = InObject;
	glGenBuffers(1, &PosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
	glBufferData(GL_ARRAY_BUFFER, Object->GetPosData().size() * sizeof(BWVector3D), Object->GetPosData().data(), GL_STATIC_DRAW);

	if (Object->GetUVData().size() != 0)
	{
		glGenBuffers(1, &UVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetUVData().size() * sizeof(BWPoint2DD), Object->GetUVData().data(), GL_STATIC_DRAW);
	}

	if (Object->GetNormalData().size() != 0)
	{
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetNormalData().size() * sizeof(BWVector3D), Object->GetNormalData().data(), GL_STATIC_DRAW);
	}

	if (Object->GetLightmapUVData().size() != 0)
	{
		glGenBuffers(1, &LightMapBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, LightMapBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetLightmapUVData().size() * sizeof(BWPoint2DD), Object->GetLightmapUVData().data(), GL_STATIC_DRAW);
	}

	/*glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * IndexData.size(), IndexData.data(), GL_STATIC_DRAW);
	*/

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
	glEnableVertexAttribArray(RTRenderer->PosLoc);
	glVertexAttribPointer(RTRenderer->PosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	/*if (UVData.size() != 0)
	{
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glEnableVertexAttribArray(RTRenderer->UVLoc);
	glVertexAttribPointer(RTRenderer->UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}*/
	if (Object->GetLightmapUVData().size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, LightMapBuffer);
		glEnableVertexAttribArray(RTRenderer->UVLoc);
		glVertexAttribPointer(RTRenderer->UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	if (Object->GetNormalData().size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glEnableVertexAttribArray(RTRenderer->NormalLoc);
		glVertexAttribPointer(RTRenderer->NormalLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}
}

template<typename ObjectType>
void RealTimeRenderable<ObjectType>::InitRealTimeRenderable(const ObjectType *InObject, GLSLProgram *Program)
{
	this->Program = Program;
	InitRealTimeRenderable(InObject, Program->PositionLoc, Program->NormalLoc, Program->UVLoc);
	GenerateBaseColor();
	GenerateNormalMap();
	GenerateMaterialTexture();
}
template<typename ObjectType>
void RealTimeRenderable<ObjectType>::InitRealTimeRenderable(const ObjectType *InObject, GLuint PosLoc, GLuint NormalLoc, GLuint UVLoc)
{
	if (InObject == nullptr) return;

	Object = InObject;
	glGenBuffers(1, &PosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
	glBufferData(GL_ARRAY_BUFFER, Object->GetPosData().size() * sizeof(BWVector3D), Object->GetPosData().data(), GL_STATIC_DRAW);

	if (Object->GetUVData().size() != 0)
	{
		glGenBuffers(1, &UVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetUVData().size() * sizeof(BWPoint2DD), Object->GetUVData().data(), GL_STATIC_DRAW);
	}

	if (Object->GetNormalData().size() != 0)
	{
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetNormalData().size() * sizeof(BWVector3D), Object->GetNormalData().data(), GL_STATIC_DRAW);
	}

	if (Object->GetLightmapUVData().size() != 0)
	{
		glGenBuffers(1, &LightMapBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, LightMapBuffer);
		glBufferData(GL_ARRAY_BUFFER, Object->GetLightmapUVData().size() * sizeof(BWPoint2DD), Object->GetLightmapUVData().data(), GL_STATIC_DRAW);
	}

	/*glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * IndexData.size(), IndexData.data(), GL_STATIC_DRAW);
	*/

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
	glEnableVertexAttribArray(PosLoc);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	if (Object->GetUVData().size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
		glEnableVertexAttribArray(UVLoc);
		glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	/*if (Object->GetLightmapUVData().size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, LightMapBuffer);
		glEnableVertexAttribArray(UVLoc);
		glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}*/
	if (Object->GetNormalData().size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glEnableVertexAttribArray(NormalLoc);
		glVertexAttribPointer(NormalLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}
}