_loc1 = dofus.DofusLoader.prototype;
_loc1.loadBitmapSmoothed = function(url, target)
{
   var _loc6_ = target.createEmptyMovieClip("bmc",target.getNextHighestDepth());
   var _loc2_ = new Object();
   _loc2_.tmc = target;
   _loc2_.onLoadInit = function(mc)
   {
      mc._visible = false;
      mc.forceSmoothing = true;
      var _loc3_ = new flash.display.BitmapData(mc._width,mc._height,true);
      this.tmc.attachBitmap(_loc3_,this.tmc.getNextHighestDepth(),"auto",true);
      _loc3_.draw(mc);
   };
   var _loc4_ = new MovieClipLoader();
   _loc4_.addListener(_loc2_);
   _loc4_.loadClip(url,_loc6_);
};
_loc1.prepareReboot = function()
{
   this.nCoreModulesLoadingStep = undefined;
   var _loc3_ = 0;
   while(_loc3_ < dofus.Constants.CORE_MODULES_LIST.length)
   {
      this._mclLoader.unloadClip(_global["MODULE_" + dofus.Constants.CORE_MODULES_LIST[_loc3_][4]]);
      _loc3_ = _loc3_ + 1;
   }
   this.reboot();
};
_loc1.addLoadingBannersFiles = function(bShow)
{
   var xDoc = new XML();
   xDoc.bShow = bShow;
   xDoc.parent = this;
   xDoc.onLoad = function(bSuccess)
   {
      if(bSuccess)
      {
         var _loc2_ = this.firstChild.firstChild;
         if(_loc2_ != null && this.childNodes.length > 0)
         {
            while(_loc2_ != null)
            {
               if(_loc2_.nodeName == "loadingbanner")
               {
                  var _loc3_ = _loc2_.attributes.file;
                  dofus.Constants.LOADING_BANNERS_FILES.push(_loc3_);
               }
               _loc2_ = _loc2_.nextSibling;
            }
         }
      }
      xDoc.parent.showBanner(xDoc.bShow);
   };
   xDoc.ignoreWhite = true;
   xDoc.load(dofus.Constants.XML_LOADING_BANNERS_PATH);
};
_loc1.showBanner = function(bShow)
{
   if(this.bLoadingBannersFilesAdded == undefined || !this.bLoadingBannersFilesAdded)
   {
      this.bLoadingBannersFilesAdded = true;
      this.addLoadingBannersFiles(bShow);
      return undefined;
   }
   var _loc4_ = bShow != undefined?bShow:!this._bBannerDisplay;
   if(_loc4_)
   {
      if(this._bBannerDisplay)
      {
         if(dofus.Constants.LOADING_BANNERS_FILES.length > 0)
         {
            this._mcBanner.swapDepths(this._mcBannerPlacer);
            this._mcBanner.removeMovieClip();
         }
         else
         {
            return undefined;
         }
      }
      var _loc3_ = undefined;
      if(dofus.Constants.LOADING_BANNERS_FILES.length > 0)
      {
         var _loc5_ = Math.floor(Math.random() * (dofus.Constants.LOADING_BANNERS_FILES.length + 1));
         if(_loc5_ < dofus.Constants.LOADING_BANNERS_FILES.length)
         {
            var _loc6_ = dofus.Constants.LOADING_BANNERS_FILES[_loc5_];
            _loc3_ = this.createEmptyMovieClip("_mcBanner",this.getNextHighestDepth());
            this.loadBitmapSmoothed(dofus.Constants.LOADING_BANNERS_PATH + _loc6_,_loc3_);
         }
      }
      if(!_loc3_)
      {
         _loc3_ = this.attachMovie("LoadingBanner_" + _global.CONFIG.language,"_mcBanner",this.getNextHighestDepth(),this._mcBannerPlacer);
      }
      if(!_loc3_)
      {
         _loc3_ = this.attachMovie("LoadingBanner","_mcBanner",this.getNextHighestDepth(),this._mcBannerPlacer);
      }
      _loc3_.cacheAsBitmap = true;
      _loc3_.swapDepths(this._mcBannerPlacer);
   }
   else
   {
      if(!this._bBannerDisplay)
      {
         return undefined;
      }
      this._mcBanner.swapDepths(this._mcBannerPlacer);
      this._mcBanner.removeMovieClip();
   }
   this._bBannerDisplay = _loc4_;
};
_loc1.onCoreLoaded = function(mcCore)
{
   if(this.nCoreModulesLoadingStep == undefined || this.nCoreModulesLoadingStep != 2)
   {
      this.showBasicInformations();
      this.showBanner(true);
   }
   _global.clearInterval(this._nFinishCoreLoadingTimer);
   this._nFinishCoreLoadingTimer = _global.setInterval(this,"onCoreLoadedFinish",1000,mcCore);
};
_loc1.onCoreLoadedFinish = function(mcCore)
{
   _global.clearInterval(this._nFinishCoreLoadingTimer);
   if(_global.CONFIG.isStreaming)
   {
      this._bJsTimer = false;
   }
   if((this._bNonCriticalError || this._bUpdate) && dofus.Constants.DEBUG)
   {
      this.showNextButton(true);
      this.showCopyLogsButton(true);
      this.showShowLogsButton(true);
   }
   else
   {
      this.initCore(mcCore);
   }
};
_loc1.addCoreModules = function()
{
   var xDoc = new XML();
   xDoc.parent = this;
   xDoc.onLoad = function(bSuccess)
   {
      if(bSuccess)
      {
         var _loc2_ = this.firstChild.firstChild;
         if(_loc2_ != null && this.childNodes.length > 0)
         {
            while(_loc2_ != null)
            {
               if(_loc2_.nodeName == "module")
               {
                  var _loc4_ = _loc2_.attributes.name;
                  var _loc3_ = _loc2_.attributes.file;
                  dofus.Constants.CORE_MODULES_LIST.push([_loc3_,dofus.Constants.MODULE_PATH + _loc3_,dofus.Constants.CORE_MODULES_LIST.length + 1,false,_loc4_]);
               }
               _loc2_ = _loc2_.nextSibling;
            }
         }
      }
      xDoc.parent.loadNextModule();
   };
   xDoc.ignoreWhite = true;
   xDoc.load(dofus.Constants.XML_CORE_MODULES_PATH);
};
_loc1.loadNextModule = function()
{
   if(dofus.Constants.ADD_CORE_MODULES && this._aCurrentModules.length < 1)
   {
      if(this.nCoreModulesLoadingStep == undefined)
      {
         this.nCoreModulesLoadingStep = 0;
      }
      switch(this.nCoreModulesLoadingStep)
      {
         case 0:
            this.nCoreModulesLoadingStep = 1;
            this.addCoreModules();
            return undefined;
            break;
         case 1:
            if(dofus.Constants.CORE_MODULES_LIST.length > 0)
            {
               this.nCoreModulesLoadingStep = 2;
               this.showBasicInformations();
               this.showBanner(true);
               this._aCurrentModules = dofus.Constants.CORE_MODULES_LIST.slice(0);
            }
      }
   }
   if(this._aCurrentModules.length < 1)
   {
      this.logTitle(this.getText("INIT_END"));
      this.onCoreLoaded(_global.MODULE_CORE);
      return undefined;
   }
   this._aCurrentModule = this._aCurrentModules.shift();
   var _loc6_ = this._aCurrentModule[0];
   var _loc4_ = this._aCurrentModule[1];
   var _loc5_ = this._aCurrentModule[2];
   var _loc3_ = this._aCurrentModule[4];
   this._mcCurrentModule = this._mcModules.createEmptyMovieClip("mc" + _loc3_,this._mcModules.getNextHighestDepth());
   this._timedProgress = _global.setInterval(this.onTimedProgress,1000,this,this._mclLoader,this._mcCurrentModule);
   this._mclLoader.loadClip(_loc4_,this._mcCurrentModule);
};
_loc1.reloadCoreModules = function()
{
   dofus.Constants.CORE_MODULES_INFOS.removeAll();
   var _loc6_ = 0;
   while(_loc6_ < dofus.Constants.CORE_MODULES_LIST.length)
   {
      var _loc3_ = dofus.Constants.CORE_MODULES_LIST[_loc6_];
      var _loc4_ = new Object();
      _loc4_.name = _loc3_[4];
      _loc4_.onLoadComplete = function(mc)
      {
         _global["MODULE_" + this.name] = mc;
         _global.API.kernel.showMessage(undefined,"Module reloaded : " + this.name,"INFO_CHAT");
      };
      var _loc5_ = new MovieClipLoader();
      _loc5_.addListener(_loc4_);
      _loc5_.unloadClip(_global["MODULE_" + _loc3_[4]]);
      var _loc7_ = this._mcModules.createEmptyMovieClip("mc" + _loc3_[4],this._mcModules.getNextHighestDepth());
      _loc5_.loadClip(_loc3_[1],_loc7_);
      _loc6_ = _loc6_ + 1;
   }
};
_global.getOccurencesSharedObject = function()
{
   return SharedObject.getLocal(dofus.Constants.GLOBAL_SO_OCCURENCES_NAME);
};
_loc1.checkOccurences = function()
{
   var _loc8_ = _global.API.lang.getConfigText("MAXIMUM_CLIENT_OCCURENCES");
   if(_loc8_ == undefined || (_global.isNaN(_loc8_) || _loc8_ < 1))
   {
      return true;
   }
   var _loc5_ = new Array();
   var _loc4_ = 1;
   while(_loc4_ < 1000)
   {
      _loc5_.push(_loc4_);
      _loc4_ = _loc4_ + 1;
   }
   var _loc6_ = _global.getOccurencesSharedObject().data.occ;
   var _loc7_ = new Array();
   _loc4_ = 0;
   while(_loc4_ < _loc6_.length)
   {
      if(_loc6_[_loc4_].tick + dofus.Constants.MAX_OCCURENCE_DELAY > new Date().getTime() && _loc6_[_loc4_].id != _global._nOccurenceId)
      {
         _loc7_.push(_loc6_[_loc4_]);
         var _loc3_ = 0;
         while(_loc3_ < _loc5_.length)
         {
            if(_loc6_[_loc4_].id == _loc5_[_loc3_])
            {
               delete register5.register3;
               break;
            }
            _loc3_ = _loc3_ + 1;
         }
      }
      _loc4_ = _loc4_ + 1;
   }
   if(_loc5_.length > 0)
   {
      _global._nOccurenceId = _loc5_[Math.floor(Math.random() * _loc5_.length)];
      _loc7_.push({id:_global._nOccurenceId,tick:new Date().getTime()});
      var _loc9_ = _loc7_.length;
   }
   if(_loc5_.length == 0 || !_global.API.datacenter.Player.isAuthorized && _loc9_ > _loc8_)
   {
      this.criticalError("TOO_MANY_OCCURENCES",this.TABULATION,false);
      return false;
   }
   _global.getOccurencesSharedObject().data.occ = _loc7_;
   _global.getOccurencesSharedObject().flush();
   this.refreshOccurenceTick();
   _global.setInterval(this,"refreshOccurenceTick",dofus.Constants.OCCURENCE_REFRESH);
   return true;
};
_loc1.refreshOccurenceTick = function()
{
   var _loc3_ = _global.getOccurencesSharedObject().data.occ;
   var _loc2_ = 0;
   while(_loc2_ < _loc3_.length)
   {
      if(_loc3_[_loc2_].id == _global._nOccurenceId)
      {
         _loc3_[_loc2_].tick = new Date().getTime();
         _loc3_[_loc2_].inGame = _global.API.datacenter.Basics.inGame;
         break;
      }
      _loc2_ = _loc2_ + 1;
   }
   _global.getOccurencesSharedObject().data.occ = _loc3_;
   _global.getOccurencesSharedObject().flush();
};
