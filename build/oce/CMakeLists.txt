
project(oce)

add_definitions(-D_OCC64)
if(ORK_BUILD_TBB)
else()
	add_definitions(-DIGNORE_NO_ATOMICS)
endif()
add_definitions(-DOCE_DISABLE_TKSERVICE_FONT)#AIS_Dimension.cxx uses this flag to remove font-dependent code
add_definitions(-DHAVE_OCE_PATHS_H)
add_definitions(-DHAVE_CONFIG_H)
if(MSVC)
	add_definitions(-DWNT -DCSFDB)
	add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244") #Narrowing conversion
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4312") #Unfortunate reinterpret casts
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800") #Coercion to bool
endif()
set(OCE_ENABLE_DEB_FLAG OFF)#Debug builds link debug OCC?
if(OCE_ENABLE_DEB_FLAG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} OCCT_DEBUG=1")
endif()


set(OCE_BIN_BUILD_INC_DIR "${3P_OCE_BIN_DIR}/build_inc")
set(OCE_BIN_INSTALL_INC_DIR "${3P_OCE_BIN_DIR}/install_inc")
set(OCE_INSTALL_INC_DIR "${INSTALL_INC_DIR}/oce")


#Header used only to build OCE
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_build_config.h.cmake"
	"${OCE_BIN_BUILD_INC_DIR}/oce-config.h"
)
#Header installed on the system
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_install_config.h.cmake"
	"${OCE_BIN_INSTALL_INC_DIR}/oce-config.h"
)
#installation paths are written into a separate file (which is not installed)
#so that only few files have to be recompiled when installation paths are modified
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_build_paths.h.cmake"
	"${OCE_BIN_INSTALL_INC_DIR}/oce-paths.h"
)


set(OCE_CONFIG_HDRS
	"${OCE_BIN_INSTALL_INC_DIR}/oce-config.h"
)
file(GLOB OCE_PUBLIC_HDRS
	${3P_OCE_SRC_DIR}/inc/*.h
	${3P_OCE_SRC_DIR}/inc/*.hxx
	${3P_OCE_SRC_DIR}/inc/*.gxx
	${3P_OCE_SRC_DIR}/inc/*.lxx
)
list(APPEND OCE_PUBLIC_HDRS "${OCE_CONFIG_HDRS}")
set(OCE_SRCS)

set(OCE_MODULES
#TKBO
	IntTools
	BRepAlgoAPI
	BOPCol
	BOPDS
	BOPAlgo
	BOPTools
#TKBool
	TopOpeBRep
	TopOpeBRepDS
	TopOpeBRepBuild
	TopOpeBRepTool
	BRepAlgo
	BRepFill
	BRepProj
#TKBRep
	TopoDS
	TopExp
	TopTools
	BRep
	BRepLProp
	BRepAdaptor
	BRepTools
#TKernel
	FSD
	MMgt
	OSD
	Plugin
	Quantity
	Resource
	SortTools
	Standard
	StdFail
	Storage
	TColStd
	TCollection
	TShort
	Units
	UnitsAPI
	Dico
	NCollection
	Message
#TKG2d
	Geom2d
	LProp
	TColGeom2d
	Adaptor2d
	Geom2dLProp
	Geom2dAdaptor
	GProp
#TKG3d
	Geom
	TColGeom
	GeomAdaptor
	AdvApprox
	GeomLProp
	Adaptor3d
	LProp3d
	TopAbs
#TKGeomBase
	ProjLib
	GeomProjLib
	GCPnts
	CPnts
	Approx
	AppParCurves
	FEmTool
	AppCont
	Extrema
	IntAna
	IntAna2d
	GeomConvert
	AdvApp2Var
	GeomLib
	Geom2dConvert
	Hermit
	BndLib
	AppDef
	GeomTools
	GC
	GCE2d
	gce
#TKGeomAlgo
	Hatch
	GeomInt
	IntStart
	IntWalk
	IntImp
	IntCurveSurface
	IntSurf
	IntPatch
	Geom2dInt
	IntImpParGen
	IntRes2d
	IntCurve
	TopTrans
	Intf
	ApproxInt
	GccAna
	GccEnt
	GccInt
	HatchGen
	Geom2dHatch
	Law
	AppBlend
	Plate
	GeomPlate
	LocalAnalysis
	GeomAPI
	GeomFill
	Geom2dAPI
	Geom2dGcc
	FairCurve
	NLPlate
	IntPolyh
	TopClass
#TKIGES
	IGESData
	IGESFile
	IGESBasic
	IGESGraph
	IGESGeom
	IGESDimen
	IGESDraw
	IGESSolid
	IGESDefs
	IGESAppli
	IGESConvGeom
	IGESSelect
	IGESToBRep
	GeomToIGES
	Geom2dToIGES
	BRepToIGES
	BRepToIGESBRep
	IGESControl
#TKLCAF
	TDF
	TDataStd
	TFunction
	TDocStd
	AppStdL
#TKMath
	math
	ElCLib
	ElSLib
	BSplCLib
	BSplSLib
	PLib
	Precision
	GeomAbs
	Poly
	CSLib
	Convert
	Bnd
	BVH
	gp
	TColgp
	TopLoc
	Expr
	ExprIntrp
#TKMesh
	BRepMesh
#TKOffset
	BRepOffsetAPI
	Draft
	BRepOffset
	BiTgte
#TKPrim
	BRepPrim
	BRepSweep
	Sweep
	BRepPrimAPI
#TKShHealing
	ShapeBuild
	ShapeExtend
	ShapeConstruct
	ShapeCustom
	ShapeAnalysis
	ShapeFix
	ShapeUpgrade
	ShapeAlgo
	ShapeProcess
	ShapeProcessAPI
#TKSTEP
	StepAP214
	RWStepAP214
	StepAP203
	RWStepAP203
	STEPConstruct
	STEPEdit
	GeomToStep
	StepToGeom
	StepToTopoDS
	TopoDSToStep
	STEPControl
	STEPSelections
	StepAP209
#TKSTEP209
	StepElement
	StepFEA
	RWStepElement
	RWStepFEA
#TKSTEPAttr
	StepVisual
	RWStepVisual
	StepDimTol
	RWStepDimTol
#TKSTEPBase
	StepBasic
	RWStepBasic
	StepRepr
	RWStepRepr
	StepGeom
	RWStepGeom
	StepShape
	RWStepShape
#TKSTL
	StlMesh
	StlAPI
	StlTransfer
	RWStl
#TKTopAlgo
	IntCurvesFace
	MAT
	MAT2d
	Bisector
	BRepMAT2d
	BRepCheck
	BRepBndLib
	BRepExtrema
	BRepClass
	BRepClass3d
	BRepLib
	BRepGProp
	BRepIntCurveSurface
	BRepTopAdaptor
	BRepBuilderAPI
	BRepApprox
#TKXSBase
	Interface
	Transfer
	IFGraph
	IFSelect
	TransferBRep
	XSControl
	StepData
	StepFile
	HeaderSection
	RWHeaderSection
	APIHeaderSection
	StepSelect
	UnitsMethods
	XSAlgo
	LibCtl
	MoniTool
)

#The spaghetti heap of OCE includes some surprising headers
#No sysmbols are actually used from these modules
set(OCE_HEADERS
#TKCDF
	#CDM
	#PCDM
	#CDF
	#UTL
	#LDOM
#TKV3d
	#V3d
	#Graphic3d
	#Visual3d
	#Select3D
	#Prs3d
	#StdPrs
	#SelectMgr
	#PrsMgr
	#AIS
	#DsgPrs
	#StdSelect
)

set(OCE_INCLUDE_DIRECTORIES
	"${OCE_BIN_BUILD_INC_DIR}" #Order matters for config header
	"${OCE_BIN_INSTALL_INC_DIR}"
	"${3P_OCE_SRC_DIR}/inc"
)
macro(process_module modules build_src build_inc install_inc)
	foreach(module ${modules})
		if(NOT IS_DIRECTORY "${3P_OCE_SRC_DIR}/src/${module}")
			message(FATAL_ERROR "Source directory does not exist: ${3P_OCE_SRC_DIR}/src/${module}")
		endif()
		if(${build_src})
			file(GLOB module_srcs
				"${3P_OCE_SRC_DIR}/src/${module}/*.cxx"
				"${3P_OCE_SRC_DIR}/src/${module}/*.c"
				"${3P_OCE_SRC_DIR}/drv/${module}/*.cxx"
				"${3P_OCE_SRC_DIR}/drv/${module}/*.c"
			)
			list(APPEND OCE_SRCS ${module_srcs})
			add_definitions("-D__${module}_DLL")
			foreach(src ${module_srcs})
				source_group("${module}" FILES "${src}")
			endforeach()
		endif()
		
		if(${build_inc} OR ${install_inc})
			file(GLOB module_hdrs
				"${3P_OCE_SRC_DIR}/src/${module}/*.h"
				"${3P_OCE_SRC_DIR}/src/${module}/*.hxx"
				"${3P_OCE_SRC_DIR}/src/${module}/*.gxx"
				"${3P_OCE_SRC_DIR}/src/${module}/*.lxx"
			)
			if(${build_inc})
				list(APPEND OCE_INCLUDE_DIRECTORIES "${3P_OCE_SRC_DIR}/src/${module}")
				if(IS_DIRECTORY "${3P_OCE_SRC_DIR}/drv/${module}")
					list(APPEND OCE_INCLUDE_DIRECTORIES "${3P_OCE_SRC_DIR}/drv/${module}")
				endif()
			endif()
			if(${install_inc})
				list(APPEND OCE_PUBLIC_HDRS ${module_hdrs})
			endif()
		endif()
	endforeach()
endmacro()

process_module("${OCE_MODULES}" ON ON ON)
process_module("${OCE_HEADERS}" OFF ON ON)


add_library(oce SHARED ${OCE_SRCS}) #${OCE_PUBLIC_HDRS}


#set_property(TARGET oce APPEND PROPERTY INCLUDE_DIRECTORIES "${OCE_INCLUDE_DIRECTORIES}")
target_include_directories(
	oce
	PUBLIC $<BUILD_INTERFACE:${OCE_INCLUDE_DIRECTORIES}>
	INTERFACE $<INSTALL_INTERFACE:${OCE_INSTALL_INC_DIR}>
)


if(ORK_BUILD_TBB)
	target_link_libraries(oce PRIVATE tbb)
	if(ORK_BUILD_TBB_MALLOC)
		target_link_libraries(oce PRIVATE tbbmalloc)
	endif()
endif()


install_bin(oce)
install_headers("${OCE_BIN_INSTALL_INC_DIR}/oce-config.h" "${OCE_INSTALL_INC_DIR}")
install_headers("${OCE_PUBLIC_HDRS}" "${OCE_INSTALL_INC_DIR}")
