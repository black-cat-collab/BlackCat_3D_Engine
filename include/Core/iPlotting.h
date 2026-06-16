#pragma once

namespace bc
{
        class IPlotting;
        enum class PlottingType
        {
                NONE,
                PLOTTING_STRAIGHT_ARROW,
                PLOTTING_STRAIGHT_SEGMENT_ARROW,
                PLOTTING_ATTACK_ARROW,
                PLOTTING_PINCER_ARROW,
                PLOTTING_TRIPLE_PINCER_ARROW,
                PLOTTING_LINE,
                PLOTTING_LINE_SEGMENT,
                PLOTTING_LINE_BEZIER,
                PLOTTING_LINE_ARROW,
                PLOTTING_LINE_BEZIER_ARROW,
                PLOTTING_ICON,
                PLOTTING_CIRCLE,
                PLOTTING_ELLIPSES,
                PLOTTING_NOTE,
                PLOTTING_RECTANGLE,
                PLOTTING_POLYGONAL,
                PLOTTING_MODEL,							// 模型
                PLOTTING_FENCE,
                MAX
        };

        enum class PlottingState
        {
                PLOTTING_NONE,
                PLOTTING_START,
                PLOTTING_MODIFY,
                PLOTTING_END
        };

        enum class PlottingLineStyle
        {
                LINE, DOTTED
        };

        enum class PlottingIconFaceType
        {
                PLACE_TILED,
                PLACE_VERTICAL,
                PLACE_FACETOCAMERA
        };

        typedef void (*PlottingStateCallback)(INode* pPlotting, PlottingState state);
        class PlottingParam : public DynamicLoadNodeParam
        {
        public:
                Vector4			vLineColor;			                 // 线颜色
                Vector4			vSelectedLineColor;	                // 选中线颜色
                float					fLineWidth;		        	// 线宽
                float					fPointSize;
                float                 fIconRatio;               // 图标比例
                float                 fDepth;                   //图层高度
                float                 fThick;                    //网格厚度
                float                 fAdjustOffset; 
                float                 fStep;                      // fence 分段
                Vector4			vPointColor;			         // 点颜色
                Vector4           vSelectedPointColor;           // 选中点的颜色
                Vector4           vFaceColor;                    // 面颜色
                Vector4           vSelectedFaceColor;            // 面颜色
                Vector4           vThickMeshColor;
                bool                bDepthTest;
                bool					bClickEnable;			// 是否可以点击
                bool					bHoverEnable;			// 是否可以悬浮
                bool                 bThickEnable;
                bool                 bModelEnable;
                bool                 bLockEnable;          // 是否启用锁定
                bool                 bLock;
                bool                 bClose;                   //fence 是否封闭
                std::string        sIconPath;
                std::string        sSelectedIconPath;
                std::string        sTextStr;
                std::string				strModelFile;		// 模型文件名
                std::string				strMaterialFile;	// 材质文件名
                std::vector<Vector3> vControlPoint;
                PlottingLineStyle eLineStyle;
                PlottingType   ePlottingType;
                PlottingIconFaceType eIconFaceType;
                PlottingStateCallback callback = nullptr;
                PlottingParam() : DynamicLoadNodeParam()
                {
                        nNodeType = NODE_PLOTTING;
                        vLineColor = Vector4(1.0f);
                        vSelectedLineColor = Vector4(1.0f);
                        fLineWidth = 1.0f;
                        fThick = 0.0f;
                        fIconRatio = 1.0f;
                        fPointSize = 3.0f;
                        fDepth = 10.0f;
                        fStep = 10.0f;
                        fAdjustOffset = 0.0f;
                        vPointColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
                        vSelectedPointColor = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
                        vFaceColor = Vector4(1.0f);
                        vSelectedFaceColor = Vector4(1.0f);
                        vThickMeshColor = Vector4(1.0f);
                        eIconFaceType = PlottingIconFaceType::PLACE_FACETOCAMERA;
                        eLineStyle = PlottingLineStyle::LINE;
                        bDepthTest = false;
                        bThickEnable = false;
                        bClickEnable = true;
                        bHoverEnable = true;
                        callback = nullptr;
                        bModelEnable = false;
                        bLock = false;
                        bClose = false;
                        strModelFile = "";
                        strMaterialFile = "";
                }

                PlottingParam& operator=(const NodeParamBase& rhs)
                {
                        NodeParamBase::operator=(rhs);
                        return *this;
                }

                PlottingParam& operator=(const DynamicLoadNodeParam& rhs)
                {
                        DynamicLoadNodeParam::operator=(rhs);
                        return *this;
                }

                PlottingParam& operator=(const PlottingParam& rhs)
                {
                        DynamicLoadNodeParam::operator=(rhs);

                        vLineColor = rhs.vLineColor;
                        vSelectedLineColor = rhs.vSelectedLineColor;
                        fLineWidth = rhs.fLineWidth;
                        fIconRatio = rhs.fIconRatio;
                        fPointSize = rhs.fPointSize;
                        fDepth = rhs.fDepth;
                        fAdjustOffset = rhs.fAdjustOffset;
                        fThick = rhs.fThick;
                        vFaceColor = rhs.vFaceColor;
                        vSelectedFaceColor = rhs.vSelectedFaceColor;
                        bDepthTest = rhs.bDepthTest;
                        vPointColor = rhs.vPointColor;
                        vSelectedPointColor = rhs.vSelectedPointColor;
                        vThickMeshColor = rhs.vThickMeshColor;
                        bClickEnable = rhs.bClickEnable;
                        bHoverEnable = rhs.bHoverEnable;
                        bThickEnable = rhs.bThickEnable;
                        bModelEnable = rhs.bModelEnable;
                        ePlottingType = rhs.ePlottingType;
                        vControlPoint = rhs.vControlPoint;
                        strModelFile = rhs.strModelFile;
                        strMaterialFile = rhs.strMaterialFile;
                        sIconPath = rhs.sIconPath;
                        sSelectedIconPath = rhs.sSelectedIconPath;
                        sTextStr = rhs.sTextStr;
                        callback = rhs.callback;
                        eIconFaceType = rhs.eIconFaceType;
                        bLockEnable = rhs.bLockEnable;
                        bLock = rhs.bLock;
                        eLineStyle = rhs.eLineStyle;
                        bClose = rhs.bClose;
                        fStep = rhs.fStep;
                        return *this;
                }
        };


        class IPlotting : public IComponent
        {
        public:
                virtual ~IPlotting() = default;
                virtual int	 iGetComponentType() { return COMPONENT_TYPE_PLOTTING; }

                virtual void iSetFaceColor(const Vector4& color) = 0;
                virtual void iSetPointColor(const Vector4& color) = 0;
                virtual void iSetPointSize(float size) = 0;
                virtual void iSetThick(float thick) = 0;
                virtual void iSetThickEnable(bool b) = 0;
                virtual void iSetLineColor(const Vector4& color) = 0;
                virtual void iSetSelectedFaceColor(const Vector4& color) = 0;
                virtual void iSetSelectedPointColor(const Vector4& color) = 0;
                virtual void iSetSelectedLineColor(const Vector4& color) = 0;
                virtual void iSetThickMeshColor(const Vector4& color) = 0;
                virtual void iSetLineWidth(float width) = 0;
                virtual void iSetDepth(float depth) = 0;
                virtual void iSetRatio(float ratio) = 0;
                virtual void iSetControlPoints(const std::vector<Vector3>& ctrlPoints) = 0;
                virtual bool iSetModelPath(const std::string& modelPath, const std::string& materialPath) = 0;

                virtual void iRevert() = 0;
                virtual void iRedo() = 0;
                virtual void iUndo() = 0;

                virtual void iPlottingFinish() = 0;

                virtual void iSetText(const std::string& txt) = 0;
                virtual bool iSetXml(const std::string& xmlPath) = 0;
                virtual bool iSaveXml(const std::string& xmlPath) = 0;

                virtual const std::vector<Vector3>& iGetControlPoints() = 0;
                virtual PlottingState iGetState() = 0;
        };
}

