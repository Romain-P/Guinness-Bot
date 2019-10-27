_global.dofus.graphics.gapi.controls.LadderGuildsMembersViewer = function()
{
   super();
};
dofus.graphics.gapi.controls.LadderGuildsMembersViewer.prototype = new ank.gapi.core.UIBasicComponent();
var _loc1 = _global.dofus.graphics.gapi.controls.LadderGuildsMembersViewer.prototype;
_loc1.__set__members = function(eaMembers)
{
   this._dgMembers.dataProvider = eaMembers;
};
_loc1.init = function()
{
   super.init(false,dofus.graphics.gapi.controls.LadderGuildsMembersViewer.CLASS_NAME);
   this.api = _global.API;
};
_loc1.createChildren = function()
{
   this.addToQueue({object:this,method:this.initTexts});
};
_loc1.initTexts = function()
{
   this._dgMembers.columnsNames = [this.api.lang.getText("LADDER_COLUMN_POSITION"),"",this.api.lang.getText("LADDER_COLUMN_NAME"),this.api.lang.getText("LADDER_COLUMN_LEVEL"),this.api.lang.getText("LADDER_COLUMN_DAYXP"),this.api.lang.getText("LADDER_COLUMN_TOTALXP"),"",""];
};
ASSetPropFlags(_loc1,null,1);
_loc1.CLASS_NAME = "LadderGuildsMembersViewer";
