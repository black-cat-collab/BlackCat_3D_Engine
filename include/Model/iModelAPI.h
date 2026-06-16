#pragma once

namespace bc
{
	class IModelAPI
	{
	public:
		/*
		 *	初始化
		 *  pSystemAPI	[in] 系统接口指针
		 */
		virtual void	iInitialize(ISystemAPI* pSystemAPI) = 0;

		/*
		*	关闭
		*/
		virtual void	iShutDown(const bool& bReload = false) = 0;

		/*
		 *	增加模型解析函数
		 *  pFunc	[in] 解析模型数据的函数指针
		 */
		virtual void	iAddModelLoadFunc(const char* pSuffix, ModelLoadFunc pFunc) = 0;

		/*
		 *	增加模型保存函数
		 *  pFunc	[in] 保存模型数据的函数指针
		 */
		virtual void	iAddModelSaveFunc(const char* pSuffix, ModelSaveFunc pFunc) = 0;

		/*
		 *	注册Model
		 *  strName	[in] 文件名
		 */
		virtual IModel* iRegistModel(ModelParam* tParam) = 0;

		/*
		 *	保存Model
		 *  pModel	[in] IModel指针
		 */
		virtual void	iSaveModel(const std::string& strModelFile, IModel* pModel) = 0;

		/*
		 *	释放Model
		 *  pModel	[in] IModel指针
		 */
		virtual void	iFreeModel(IModel* pModel) = 0;

		/*
		 *	获取Model数量
		 *  return	[out] 数量
		 */
		virtual int		iGetModelCount() = 0;

		/*
		 *	获取已编译的Model数量
		 *  return	[out] 数量
		 */
		virtual int		iGetCompileCount() = 0;

		
	};
}
