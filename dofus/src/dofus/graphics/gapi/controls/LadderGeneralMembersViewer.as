_global.dofus.graphics.gapi.controls.LadderGeneralMembersViewer = function()
{
   super();
};
dofus.graphics.gapi.controls.LadderGeneralMembersViewer.prototype = new ank.gapi.core.UIBasicComponent();
var _loc1 = _global.dofus.graphics.gapi.controls.LadderGeneralMembersViewer.prototype;
_loc1.__set__members = function(eaMembers)
{
   this._dgMembers.dataProvider = eaMembers;
};
_loc1.init = function()
{
   super.init(false,dofus.graphics.gapi.controls.LadderGeneralMembersViewer.CLASS_NAME);
   this.api = _global.API;
};
_loc1.createChildren = function()
{
   this.addToQueue({object:this,method:this.initTexts});
   this.addToQueue({object:this,method:this.addListeners});
};
_loc1.addListeners = function()
{
   this._btnShowOnlyFriends.addEventListener("click",this);
};
_loc1.initTexts = function()
{
   this._dgMembers.columnsNames = [this.api.lang.getText("LADDER_COLUMN_POSITION"),"",this.api.lang.getText("LADDER_COLUMN_NAME"),this.api.lang.getText("LADDER_COLUMN_LEVEL"),this.api.lang.getText("LADDER_COLUMN_DAYXP"),this.api.lang.getText("LADDER_COLUMN_TOTALXP"),"",""];
   this._lblShowOnlyFriends.text = this.api.lang.getText("LADDER_ONLYFRIENDS");
};
_loc1.click = function(oEvent)
{
   if((var _loc0_ = oEvent.target) === this._btnShowOnlyFriends)
   {
      var _loc3_ = this.api.network._oDataProcessor;
      if(this._btnShowOnlyFriends.selected)
      {
         generalLadderOnlyFriends.removeAll();
         this.api.network.send("#LDF");
      }
      else if(_global.generalLadderMembers.length > 0)
      {
         _loc3_.dispatchEvent({type:"modelChanged"});
      }
   }
};
ASSetPropFlags(_loc1,null,1);
_loc1.CLASS_NAME = "LadderGeneralMembersViewer";
