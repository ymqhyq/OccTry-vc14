# OccTry-vc14
原资源下载自https://download.csdn.net/download/astrosky/1736346
https://blog.csdn.net/astrosky/article/details/4660179
但原程序基于occt6.2，现在官网最早的版本也是6.5了，occt6.2版本找不到。
最新occt版本为7.4，很多类都重构了，一些类已经作废移除。
尝试使用occt7.4升级原occTry，居然成功了。
但中间注释掉了一些内容，注释和修改的地方使用//yxk进行了标识。
迁移至新版本的过程中主要参考了下列连接的官方帮助，上面有一些升级信息。
https://dev.opencascade.org/doc/overview/html/occt_user_guides__ocaf.html
迁移至新occt版本的主要修改包括：
找不到的头文件全部注释掉了。
Handle(Graphic3d_WNTGraphicDevice)替换为Handle(Graphic3d_GraphicDriver)
暂时移除了：ScCmdMeshGen.cpp，ScCmdMeshGen.hxx
Local Context removal
Previously deprecated Local Context functionality has been removed from AIS package, so that related methods have been removed from AIS_InteractiveContext interface: HasOpenedContext(), HighestIndex(), LocalContext(), LocalSelector(), OpenLocalContext(), CloseLocalContext(), IndexOfCurrentLocal(), CloseAllContexts(), ResetOriginalState(), ClearLocalContext(), UseDisplayedObjects(), NotUseDisplayedObjects(), SetShapeDecomposition(), SetTemporaryAttributes(), ActivateStandardMode(), DeactivateStandardMode(), KeepTemporary(), SubIntensityOn(), SubIntensityOff(), ActivatedStandardModes(), IsInLocal(), AddOrRemoveSelected() taking TopoDS_Shape.
AIS_InteractiveContext上述成员函数直接注释掉
BRepBuilderAPI_MakeFace等函数增加了限差参数。
AIS_InteractiveContext::Display函数增加了是否更新视图的参数
Select3D_Projector类已经废除，使用新的方法实现了ScView::GetEyeLine函数
yangxukun20200128