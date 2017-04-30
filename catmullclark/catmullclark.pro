INCLUDEPATH += ../shared ../lapl ./ ../easymodel ../../ofl/core ../../ofl/opium
HEADERS       = ../shared/Vector3F.h \
                ../shared/Vector2F.h \
                ../shared/Matrix44F.h \
                ../shared/Matrix33F.h \
                ../shared/Ray.h \
                ../shared/RayIntersectionContext.h \
                ../shared/IntersectionContext.h \
                ../shared/BaseCamera.h \
                ../shared/PerspectiveCamera.h \
                ../shared/Base3DView.h \
                ../shared/BaseDrawer.h \
                ../shared/KdTreeDrawer.h \
                ../shared/SpaceHandle.h \
                ../shared/TypedEntity.h \
                ../shared/Geometry.h \
                ../shared/BaseMesh.h \
                ../shared/BoundingBox.h \
                ../shared/TriangleMesh.h \
                ../shared/GeoElement.h \
                ../shared/Vertex.h \
                ../shared/Edge.h \
                ../shared/Facet.h \
                ../shared/BaseField.h \
                ../shared/BaseDeformer.h \
                ../shared/KdTreeNode.h \
                ../shared/KdTree.h \
                ../shared/Primitive.h \
                ../shared/PrimitiveFilter.h \
                ../shared/BuildKdTreeContext.h \
                ../shared/KdTreeNodeArray.h \
                ../shared/PrimitiveArray.h \
                ../shared/IndexArray.h \
                ../shared/BaseArray.h \
                ../shared/ClassificationStorage.h \
                ../shared/KdTreeBuilder.h \
                ../shared/SplitEvent.h \
                ../shared/MinMaxBins.h \
                ../shared/BuildKdTreeStream.h \
                ../shared/IndexList.h \
                ../shared/BoundingBoxList.h \
                ../shared/SelectionArray.h \
                ../shared/BarycentricCoordinate.h \
                ../shared/EasemodelUtil.h \
                ../shared/GeodesicSphereMesh.h \
				../shared/PyramidMesh.h \
                ../shared/AnchorGroup.h \
                ../shared/BaseCurve.h \
                ../shared/VertexPath.h \
                ../shared/MeshTopology.h \
                ../shared/PatchMesh.h \
                ../lapl/VertexAdjacency.h \
                ../lapl/Anchor.h \
                ../../ofl/core/BaseImage.cpp \
                ../../ofl/core/zEXRImage.h \
                LODQuad.h \
                accPatch.h \
                accStencil.h \
                bezierPatch.h \
                patchTopology.h \
                tessellator.h \
                glwidget.h \
                subdivision.h \
                window.h 
                
SOURCES       = ../shared/Vector3F.cpp \  
                ../shared/Vector2F.cpp \
                ../shared/Matrix44F.cpp \
                ../shared/Matrix33F.cpp \
                ../shared/Ray.cpp \
                ../shared/RayIntersectionContext.cpp \
                ../shared/IntersectionContext.cpp \
                ../shared/BaseCamera.cpp \
                ../shared/PerspectiveCamera.cpp \
                ../shared/Base3DView.cpp \
                ../shared/BaseDrawer.cpp \
                ../shared/KdTreeDrawer.cpp \
                ../shared/SpaceHandle.cpp \
                ../shared/TypedEntity.cpp \
                ../shared/Geometry.cpp \
                ../shared/BoundingBox.cpp \
                ../shared/BaseMesh.cpp \
                ../shared/TriangleMesh.cpp \
                ../shared/GeoElement.cpp \
                ../shared/Vertex.cpp \
                ../shared/Edge.cpp \
                ../shared/Facet.cpp \
                ../shared/BaseField.cpp \
                ../shared/BaseDeformer.cpp \
                ../shared/KdTreeNode.cpp \
                ../shared/KdTree.cpp \
                ../shared/Primitive.cpp \
                ../shared/PrimitiveFilter.cpp \
                ../shared/BuildKdTreeContext.cpp \
                ../shared/KdTreeNodeArray.cpp \
                ../shared/PrimitiveArray.cpp \
                ../shared/IndexArray.cpp \
                ../shared/BaseArray.cpp \
                ../shared/ClassificationStorage.cpp \
                ../shared/KdTreeBuilder.cpp \
                ../shared/SplitEvent.cpp \
                ../shared/MinMaxBins.cpp \
                ../shared/BuildKdTreeStream.cpp \
                ../shared/IndexList.cpp \
                ../shared/BoundingBoxList.cpp \
                ../shared/SelectionArray.cpp \
                ../shared/BarycentricCoordinate.cpp \
                ../shared/EasemodelUtil.cpp \
                ../shared/GeodesicSphereMesh.cpp \
				../shared/PyramidMesh.cpp \
                ../shared/AnchorGroup.cpp \
                ../shared/BaseCurve.cpp \
                ../shared/VertexPath.cpp \
                ../shared/MeshTopology.cpp \
                ../shared/PatchMesh.cpp \
                ../lapl/VertexAdjacency.cpp \
                ../lapl/Anchor.cpp \
                ../../ofl/core/BaseImage.cpp \
                ../../ofl/core/zEXRImage.cpp \
                LODQuad.cpp \
                accPatch.cpp \
                accStencil.cpp \
                bezierPatch.cpp \
                patchTopology.cpp \
                tessellator.cpp \
                glwidget.cpp \
                subdivision.cpp \
                window.cpp \
                main.cpp 
                
INCLUDEPATH += /usr/local/include/OpenEXR
LIBS += -leasymodel -lIlmImf -lHalf
macx {
    INCLUDEPATH += ../../Library/boost_1_44_0
        LIBS += -lboost_date_time\
            -lboost_thread
}
QT           += opengl

