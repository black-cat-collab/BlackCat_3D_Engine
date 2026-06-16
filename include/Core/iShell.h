#pragma once

namespace bc
{
	class ShellParam : public DynamicLoadNodeParam
	{
	public:
		Vector4					vBaseColor;				// 底面颜色 有图片优先使用图片
		Vector4					vSelectColor;			// 底面被选择颜色
		Vector4					vHoverColor;			// 底面鼠标悬浮颜色
		bool					bClickEnable;			// 是否可以点击
		bool					bHoverEnable;			// 是否可以悬浮
		bool                bColorEnable;         //是否显示颜色
		std::vector<Triangle>   vecTriangle;
		bool					bDepthTest;				// 深度测试
		std::string				strModelName;			// Msh模型名字 
		Vector3					vCenterPosition;
		std::string             strTexturePath;
		float           fMaxZ;
		ShellParam() : DynamicLoadNodeParam()
		{
			nNodeType = NODE_SHELL;
			vBaseColor = Vector4(1.0);
			vSelectColor = Vector4(1.0);
			vHoverColor = Vector4(1.0);
			vCenterPosition = Vector3(0.0);
			bClickEnable = false;
			bHoverEnable = true;
			bColorEnable = true;
			//fHeight = 0.0;
			vecTriangle.clear();
			bDepthTest = true;
			fMaxZ = 0.0f;
		}

		ShellParam(const std::vector<Triangle>&tri ) : DynamicLoadNodeParam(),
			vecTriangle(tri)
		
		{
			nNodeType = NODE_SHELL;
			vBaseColor = Vector4(1.0);
			vSelectColor = Vector4(1.0);
			vHoverColor = Vector4(1.0);
			vCenterPosition = Vector3(0.0);
			bClickEnable = true;
			bHoverEnable = true;
			bColorEnable = true;
			bDepthTest = true;
		}

		ShellParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		ShellParam& operator=(const ShellParam& src)
		{
			DynamicLoadNodeParam::operator=(src);

			vBaseColor = src.vBaseColor;
			vSelectColor = src.vSelectColor;
			vHoverColor = src.vHoverColor;
			bClickEnable = src.bClickEnable;
			bHoverEnable = src.bHoverEnable;
		//	fHeight = src.fHeight;
			vecTriangle.assign(src.vecTriangle.begin(), src.vecTriangle.end());
			bDepthTest = src.bDepthTest;
			strModelName = src.strModelName;
			strTexturePath = src.strTexturePath;
			vCenterPosition = src.vCenterPosition;
			return *this;
		}
	};
	
	class IShell : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_SHELL; }
		virtual void		iGetShellParam(ShellParam& tParam) = 0;
		virtual void		iSetShellParam(const ShellParam& tParam) = 0;
		virtual void        iOutputToFile(const std::string& path) = 0;
		virtual void    iSetEditorState(bool state) = 0;
		//设置文件保存文件夹路径（主要用于业务保存，不设置则使用默认）
		virtual void        iSetSaveDir(const std::string& path) = 0;
	};
} // namespace bc


