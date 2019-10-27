_global.dofus.graphics.gapi.ui.AskOkWait = function()
{
   super();
};
dofus.graphics.gapi.ui.AskOkWait.prototype = new ank.gapi.ui.FlyWindow();
_loc1 = dofus.graphics.gapi.ui.AskOkWait.prototype;
_loc1.init = function()
{
   super.init(false,dofus.graphics.gapi.ui.AskOkWait.CLASS_NAME);
};
_loc1.__set__text = function(sText)
{
   this._sText = sText;
};
_loc1.__get__text = function()
{
   return this._sText;
};
_loc1.initWindowContent = function()
{
   this.nWaitClosureDuration = 5;
   var _loc2_ = this._winBackground.content;
   _loc2_._btnOk.enabled = false;
   _loc2_._btnOk.label = this.api.lang.getText("OK") + " (" + this.nWaitClosureDuration + ")";
   _loc2_._btnOk.addEventListener("click",this);
   _loc2_._txtText.addEventListener("change",this);
   _loc2_._txtText.text = this._sText;
   Selection.setFocus(null);
   this.api.kernel.KeyManager.removeKeysListener(this.api.ui.getUIComponent("Banner"));
   this.api.kernel.KeyManager.removeShortcutsListener(this.api.ui.getUIComponent("Banner"));
   this.api.kernel.KeyManager.addShortcutsListener("onShortcut",this);
   this.startTimer();
};
_loc1.click = function(oEvent)
{
   this.api.kernel.KeyManager.addKeysListener("onKeys",this.api.ui.getUIComponent("Banner"));
   this.api.kernel.KeyManager.addShortcutsListener("onShortcut",this.api.ui.getUIComponent("Banner"));
   this.api.kernel.KeyManager.removeShortcutsListener(this);
   this.dispatchEvent({type:"ok"});
   this.unloadThis();
};
_loc1.change = function(oEvent)
{
   var _loc2_ = this._winBackground.content;
   _loc2_._btnOk._y = _loc2_._txtText._y + _loc2_._txtText.height + 20;
   this._winBackground.setPreferedSize();
};
_loc1.onShortcut = function(sShortcut)
{
   var _loc2_ = this._winBackground.content;
   if(sShortcut == "ACCEPT_CURRENT_DIALOG" && _loc2_._btnOk.enabled)
   {
      Selection.setFocus();
      this.click();
      return false;
   }
   return true;
};
_loc1.startTimer = function()
{
   this.stopTimer();
   this.nIntervalID = _global.setInterval(this,"updateTimer",1000);
};
_loc1.stopTimer = function()
{
   _global.clearInterval(this.nIntervalID);
};
_loc1.updateTimer = function()
{
   this.nWaitClosureDuration = this.nWaitClosureDuration - 1;
   var _loc2_ = this._winBackground.content;
   _loc2_._btnOk.label = this.api.lang.getText("OK") + " (" + this.nWaitClosureDuration + ")";
   if(this.nWaitClosureDuration == 0)
   {
      _loc2_._btnOk.label = this.api.lang.getText("OK");
      _loc2_._btnOk.enabled = true;
      this.stopTimer();
   }
};
_loc1.addProperty("text",_loc1.__get__text,_loc1.__set__text);
_loc1.CLASS_NAME = "AskOkWait";
ASSetPropFlags(_loc1,null,1);
