_global.dofus.graphics.gapi.controls.LadderMembersViewerMember = function()
{
   super();
};
dofus.graphics.gapi.controls.LadderMembersViewerMember.prototype = new ank.gapi.core.UIBasicComponent();
var _loc1 = _global.dofus.graphics.gapi.controls.LadderMembersViewerMember.prototype;
_loc1.__set__list = function(mcList)
{
   this._mcList = mcList;
};
_loc1.setValue = function(bUsed, sSuggested, oItem)
{
   if(bUsed)
   {
      this._oItem = oItem;
      this._lblPosition.text = oItem.position;
      var _loc3_ = oItem.emblem;
      if(_loc3_ == undefined)
      {
         this._ldrGfx.contentPath = dofus.Constants.GUILDS_MINI_PATH + oItem.gfx + ".swf";
         this._eEmblem._visible = false;
      }
      else
      {
         this._eEmblem.backID = _loc3_.backID;
         this._eEmblem.backColor = _loc3_.backColor;
         this._eEmblem.upID = _loc3_.upID;
         this._eEmblem.upColor = _loc3_.upColor;
         this._eEmblem._visible = true;
      }
      this._lblName.text = oItem.name;
      this._lblLevel.text = oItem.level;
      this._lbl24hXP.text = oItem.dayxp;
      this._lblTotalXP.text = oItem.totalxp;
   }
   else if(this._lblName.text != undefined)
   {
      this._lblPosition.text = "";
      this._eEmblem._visible = false;
      this._ldrGfx.contentPath = "";
      this._lblName.text = "";
      this._lblLevel.text = "";
      this._lbl24hXP.text = "";
      this._lblTotalXP.text = "";
   }
};
_loc1.init = function()
{
   super.init(false);
};
_loc1.addProperty("list",function()
{
}
,_loc1.__set__list);
ASSetPropFlags(_loc1,null,1);
