_global.dofus.graphics.gapi.ui.Ladder = function()
{
   super();
};
this.api = _global.API;
dofus.graphics.gapi.ui.Ladder.prototype = new ank.gapi.core.UIBasicComponent();
_loc1 = dofus.graphics.gapi.ui.Ladder.prototype;
_loc1.init = function()
{
   super.init(false,dofus.graphics.gapi.ui.Ladder.CLASS_NAME);
};
_loc1.callClose = function()
{
   this.gapi.unloadUIComponent("Ladder");
   return true;
};
_loc1.createChildren = function()
{
   this.addToQueue({object:this,method:this.initTexts});
   this.addToQueue({object:this,method:this.addListeners});
   this.addToQueue({object:this,method:this.setCurrentTab,params:["General"]});
};
_loc1.initTexts = function()
{
   this._winBackground.title = "Ladder";
   servername = this.api.datacenter.Basics.aks_current_server.__get__label();
   if(servername != undefined)
   {
      this._winBackground.title = "Ladder " + servername;
   }
   this._btnTabGeneral.label = this.api.lang.getText("LADDER_TAB_GENERAL");
   this._btnTabGuilds.label = this.api.lang.getText("LADDER_TAB_GUILDS");
};
_loc1.addListeners = function()
{
   this._btnClose.addEventListener("click",this);
   this._btnTabGeneral.addEventListener("click",this);
   this._btnTabGuilds.addEventListener("click",this);
   this.api.network._oDataProcessor.addEventListener("modelChanged",this);
   this.api.network._oDataProcessor.addEventListener("modelChanged2",this);
   ank.utils.MouseEvents.addListener(this);
};
_loc1.destroy = function()
{
   this.api.network._oDataProcessor.removeEventListener("modelChanged",this);
};
_loc1.updateCurrentTabInformations = function()
{
   this._mcTabViewer.removeMovieClip();
   this.attachMovie("Ladder" + this._sCurrentTab + "MembersViewer","_mcTabViewer",this.getNextHighestDepth(),{_x:this._mcPlacer._x,_y:this._mcPlacer._y});
   this._mcTabViewer.setMask(this._mcMask);
   this.addToQueue({object:this,method:this.initData});
};
_loc1.initData = function()
{
   switch(this._sCurrentTab)
   {
      case "General":
         if(_global.generalLadderMembers.length == 0)
         {
            this.api.network.send("#LD1");
         }
         else
         {
            this._mcTabViewer.__set__members(_global.generalLadderMembers);
         }
         break;
      case "Guilds":
         if(_global.guildsLadderMembers.length == 0)
         {
            this.api.network.send("#LD2");
         }
         else
         {
            this._mcTabViewer.__set__members(_global.guildsLadderMembers);
         }
   }
};
_loc1.setCurrentTab = function(sNewTab)
{
   this._mcComboBoxPopup.removeMovieClip();
   var _loc3_ = this["_btnTab" + this._sCurrentTab];
   var _loc2_ = this["_btnTab" + sNewTab];
   _loc3_.selected = true;
   _loc3_.enabled = true;
   _loc2_.selected = false;
   _loc2_.enabled = false;
   this._sCurrentTab = sNewTab;
   this._sbOptions.scrollPosition = 0;
   this.updateCurrentTabInformations();
};
_loc1.click = function(oEvent)
{
   switch(oEvent.target._name)
   {
      case "_btnTabGeneral":
      case "_btnTabGuilds":
         this.setCurrentTab(oEvent.target._name.substr(7));
         break;
      case "_btnClose":
         this.callClose();
   }
};
_loc1.modelChanged = function(oEvent)
{
   if(this._sCurrentTab == "General")
   {
      this._mcTabViewer.__set__members(_global.generalLadderMembers);
   }
   else
   {
      this._mcTabViewer.__set__members(_global.guildsLadderMembers);
   }
};
_loc1.modelChanged2 = function(oEvent)
{
   if(this._sCurrentTab == "General")
   {
      this._mcTabViewer.__set__members(_global.generalLadderOnlyFriends);
   }
};
ASSetPropFlags(_loc1,null,1);
dofus.graphics.gapi.ui.Ladder.CLASS_NAME = "Ladder";
