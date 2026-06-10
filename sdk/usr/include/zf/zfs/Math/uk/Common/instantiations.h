//FH #include "XYZim/listVector.h"
//#include "container.h"

//FH #include "ThreeDraw/3DrawClass.h"
//FH #include "ThreeDraw/3DrawSingle.h"
//FH #include "Objects/3DEntity.h"
//FH #include "Model/vkNode.h"
//#include "entit.h"
//#include "mLayerClass.h"
//#include "mLayerClass2.h"
//#include "scrollingLeaf.h"
//#include "colour.h"
//#include "modelClass.h"
//#include "readData.h"
//#include "listingLeaf.h"
//#include "modelViewpoint.h"
//#include "modelNode.h"
//FH #include "XYZim/subMask.h"
//#include "cloudMenu.h"
//#include "Tree.h"
//FH #include "XYZim/XYZimThree.h"
//FH #include "Objects/pickCloud.h"
//#include "treeObject.h"
//FH #include "Mesh/modelMesh.h"
//#include "userProfiles.h"
//FH #include "Mesh/cpoint.h"
//FH #include "Maths/Vector/boundaryPoint.h"
//FH #include "Scroll/scroll.h"
//FH #include "Mesh/deciTriangle.h"
//FH #include "Mesh/deciFunctions.h"
//FH #include "Mesh/deciVertex.h"
//FH #include "LFMFunc/container.h"
//FH #include "LFMFunc/containerModel.h"
//FH #include "Base/genericListViewItem.h"

//FH #include "ThreeDraw/3DrawMulti.h"
//FH #include "FloorPlan/dxfEntity.h"
//FH #include "FloorPlan/dxfDrawing.h"
//FH #include "FloorPlan/dxfViewer.h"
//FH #include "FloorPlan/dxfReader.h"
//FH #include "FloorPlan/dxfHooked.h"
//FH #include "FloorPlan/dxfPicture.h"
//#include "functions/xEventL.h"

//#include "DXFclasses.h"
//#include "dxfReader.h"
//#include "dxfFloorPlan.h"


class ListVector;
class ContainerModel;
class Container;
class Entit;
class VkNode;
class mLayer_ListItem;
class scrollingLeaf_listItem;
class ThreeDEntity;
class Global3DEntityL;
class modelViewpoint_listItem;
class SubMask;
class XYZim;
class SubCloud;
//FH #include  "ModellingControl/modellingControl.h"
//FH #include  "ModellingControl/modellingControlPopUp.h"
using std::cerr;
using std::cout;
using std::endl;
using std::flush;

template class Node<ListVector>;
template class MyNode<ListVector>;
template class List<ListVector>;

/*FH
template class Node<ContainerModel>;
template class MyNode<ContainerModel>;
template class List<ContainerModel>;

template class Node<Container>;
template class MyNode<Container>;
template class List<Container>;
*/
//template class MyNode<Entit>;
//template class List<Entit>;
/*FH
template class Node<ThreeDEntity>;
template class MyNode<ThreeDEntity>;
template class List<ThreeDEntity>;
template class MyNode<VkNode>;
template class List<VkNode>;
*/
//template class Node<mLayer_ListItem>;
//template class MyNode<mLayer_ListItem>;
//template class List<mLayer_ListItem>;
//template class MyNode<scrollingLeaf_listItem>;
//template class List<scrollingLeaf_listItem>;
//template class MyNode<PointsColour>;
//template class List<PointsColour>;
/*FH
template class Node<Point>;
template class MyNode<Point>;
template class List<Point>;
*/
//template class MyNode<threeDrawMulti_c>;
//template class List<threeDrawMulti_c>;
/*FH
template class MyNode<threeDrawSingle_c>;
template class List<threeDrawSingle_c>;
template class Node<SubMask>;
template class MyNode<SubMask>;
template class List<SubMask>;
*/
//template class Node<mLayer2_ListItem>;
//template class MyNode<mLayer2_ListItem>;
//template class List<mLayer2_ListItem>;
//template class MyNode<listingLeaf_listItem>;
//template class List<listingLeaf_listItem>;
//template class MyNode<dataItem>;
//template class List<dataItem>;
//template class Node<modelViewpoint_listItem>;
//template class MyNode<modelViewpoint_listItem>;
//template class List<modelViewpoint_listItem>;
//template class Node<modelNode_listItem>;
//template class MyNode<modelNode_listItem>;
//template class List<modelNode_listItem>;
//template class Node<tree_listItem>;
//template class MyNode<tree_listItem>;
//template class List<tree_listItem>;
/*FH
template class Node<XYZim>;
template class MyNode<XYZim>;
template class List<XYZim>;
template class Node<SubCloud>;
template class MyNode<SubCloud>;
template class List<SubCloud>;
*/
//template class Node<treeObject_c>;
//template class MyNode<treeObject_c>;
//template class List<treeObject_c>;

/*FH
template class Node<VertexL>;
template class MyNode<VertexL>;
template class List<VertexL>;
template class Node<EdgeL>;
template class MyNode<EdgeL>;
template class List<EdgeL>;
template class  Node<TriangL>;
template class  MyNode<TriangL>;
template class  List<TriangL>;

template class Node<MeshCore>;
template class MyNode<MeshCore>;
template class List<MeshCore>;
*/

//template class Node<userProfile_listItem>;
//template class MyNode<userProfile_listItem>;
//template class List<userProfile_listItem>;

/*FH
template class Node<Cpoint>;
template class MyNode<Cpoint>;
template class List<Cpoint>;

template class Node<BoundaryPoint>;
template class MyNode<BoundaryPoint>;
template class List<BoundaryPoint>;

template class Node<PartialBox_c>;
template class MyNode<PartialBox_c>;
template class List<PartialBox_c>;

template class Node<PartialBox>;
template class MyNode<PartialBox>;
template class List<PartialBox>;

template class Node<PartialInfo_c>;
template class MyNode<PartialInfo_c>;
template class List<PartialInfo_c>;

template class Node<ImageMask>;
template class MyNode<ImageMask>;
template class List<ImageMask>;

template class Node<ImageNodesListView>;
template class MyNode<ImageNodesListView>;
template class List<ImageNodesListView>;

template class Node<ModControlViewpoint>;
template class MyNode<ModControlViewpoint>;
template class List<ModControlViewpoint>;

template class Node<GenListView>;
template class MyNode<GenListView>;
template class List<GenListView>;

template class Node<ModellingControlViewpoint>;
template class MyNode<ModellingControlViewpoint>;
template class List<ModellingControlViewpoint>;

//template class Node<mLayer2pulldown_ListItem>;
//template class MyNode<mLayer2pulldown_ListItem>;
//template class List<mLayer2pulldown_ListItem>;
//  floorPlan (added)
//template class Node<DXFDrawing>;
//template class MyNode<DXFDrawing>;
//template class List<DXFDrawing>;
//template class Node<DXFEntity>;
//template class MyNode<DXFEntity>;
//template class List<DXFEntity>;
//template class Node<DXFBlock>;
//template class MyNode<DXFBlock>;
//template class List<DXFBlock>;
//template class Node<GIFImage>;
//template class MyNode<GIFImage>;
//template class List<GIFImage>;
//template class Node<DXFFloorPlanButton_ListItem>;
//template class MyNode<DXFFloorPlanButton_ListItem>;
//template class List<DXFFloorPlanButton_ListItem>;
//template class Node<DXFLType>;
//template class MyNode<DXFLType>;
//template class List<DXFLType>;
//template class Node<DXFLayer>;
//template class MyNode<DXFLayer>;
//template class List<DXFLayer>;
//template class Node<DXFFloorPlanOrphan>;
//template class MyNode<DXFFloorPlanOrphan>;
//template class List<DXFFloorPlanOrphan>;

template class Node<DXFLayer>;
template class MyNode<DXFLayer>;
template class List<DXFLayer>;

template class Node<DXFLType>;
template class MyNode<DXFLType>;
template class List<DXFLType>;

template class Node<DXFEntity>;
template class MyNode<DXFEntity>;
template class List<DXFEntity>;

template class Node<DXFBlock>;
template class MyNode<DXFBlock>;
template class List<DXFBlock>;

template class Node<DXFDrawing>;
template class MyNode<DXFDrawing>;
template class List<DXFDrawing>;

template class Node<threeDrawMulti_c>;
template class MyNode<threeDrawMulti_c>;
template class List<threeDrawMulti_c>;

template class Node<DXFHooked>;
template class MyNode<DXFHooked>;
template class List<DXFHooked>;

template class Node<DXFPictureData>;
template class MyNode<DXFPictureData>;
template class List<DXFPictureData>;
*/

/*
template class MyNode<>;
template class List<>;
template class MyNode<>;
template class List<>;
template class MyNode<>;
template class List<>;
template class MyNode<>;
template class List<>;
template class MyNode<>;
template class List<>;
*/
