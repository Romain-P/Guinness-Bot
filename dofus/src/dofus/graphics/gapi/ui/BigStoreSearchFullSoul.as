_global.dofus.graphics.gapi.ui.BigStoreSearchFullSoul = function()
{
   super();
};
this.api = _global.API;
dofus.graphics.gapi.ui.BigStoreSearchFullSoul.prototype = new ank.gapi.core.UIBasicComponent();
_loc1 = dofus.graphics.gapi.ui.BigStoreSearchFullSoul.prototype;
_loc1.__set__oParent = function(o)
{
   this._oParent = o;
};
_loc1.init = function()
{
   super.init(false,dofus.graphics.gapi.ui.BigStoreSearchFullSoul.CLASS_NAME);
};
_loc1.callClose = function()
{
   this.gapi.hideTooltip();
   this.gapi.unloadUIComponent("BigStoreSearchFullSoul");
   return true;
};
_loc1.createChildren = function()
{
   this.addToQueue({object:this,method:this.addListeners});
   this.addToQueue({object:this,method:this.initTexts});
};
_loc1.addListeners = function()
{
   this._btnClose.addEventListener("click",this);
   this._btnClose2.addEventListener("click",this);
   this._btnView.addEventListener("click",this);
};
_loc1.initTexts = function()
{
   this._winBackground.title = this.api.lang.getText("BIGSTORE_SEARCH");
   this._lblSearch.text = this.api.lang.getText("BIGSTORE_SEARCH_MONSTER_NAME");
   this._btnClose2.label = this.api.lang.getText("CLOSE");
   this._btnView.label = this.api.lang.getText("BIGSTORE_SEARCH_VIEW");
   this._tiSearch.text = "";
   this._tiSearch.setFocus();
};
_loc1.click = function(oEvent)
{
   switch(oEvent.target._name)
   {
      case "_btnClose":
      case "_btnClose2":
         this.callClose();
         break;
      case "_btnView":
         this._oParent.sFullSoulMonster = this._tiSearch.text;
         this._oParent.modelChanged2();
         this.callClose();
   }
};
_loc1.addProperty("oParent",function()
{
}
,_loc1.__set__oParent);
ASSetPropFlags(_loc1,null,1);
_global.dofus.graphics.gapi.ui.BigStoreSearchFullSoul.CLASS_NAME = "BigStoreSearchFullSoul";
