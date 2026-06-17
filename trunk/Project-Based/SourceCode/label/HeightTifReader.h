#pragma once

namespace bc
{
	class ProjectBasedClient;
	class HeightTifReader
	{
	protected:
		class InnerTaskData
		{
		public:
			std::vector<FactoryAssemble*> vecAssemble;
			std::vector<double> vecHeight;
		};

	public:
		virtual ~HeightTifReader();

		static HeightTifReader* GetInstance();
		void AddTask(FactoryAssemble* pAssemble);
		void AddFirstTask(FactoryAssemble* pAssemble);
		void FrameUpdate();
		void Clear();
		void AddFunc(IFunc *pFuncObject);
		void RemoveFunc(IFunc* pFuncObject);
		int GetTaskSize();

	protected:
		HeightTifReader();

	private:
		static void static_camera_FrameUpdate(ISystemAPI* pSystemAPI, INode* pNode, void* pUser);

		static void static_request_doingReadHeightTif(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void static_callback_doingReadHeightTif(ThreadCallbackParam_s* pParam);
		void doingReadHeightTifTread(int nIndex);		//线程计算

	private:
		static HeightTifReader* m_Instance;

		std::stack<FactoryAssemble*> m_stackTask;
		std::stack<FactoryAssemble*> m_stackFirstTask;
		ProjectBasedClient* m_pClient;
		std::vector<IFunc*> m_vecFunc;

		BCMutexLock	m_MutexLock;
		bool		m_bThreadRunning;

	private:
		class GC // 垃圾回收类
		{
		public:
			GC()
			{
				std::cout << "HeightTifReader GC construction" << std::endl;
			}

			~GC()
			{
				cout << "HeightTifReader GC destruction" << endl;
				if (m_Instance != NULL)
				{
					delete m_Instance;
					m_Instance = NULL;
					std::cout << "HeightTifReader destruction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};
}



