class ank.battlefield.datacenter.Cell extends Object
{
   var active = true;
   var lineOfSight = true;
   var layerGroundRot = 0;
   var groundLevel = 7;
   var movement = 4;
   var layerGroundNum = 0;
   var groundSlope = 1;
   var layerGroundFlip = false;
   var layerObject1Num = 0;
   var layerObject1Rot = 0;
   var layerObject1Flip = false;
   var layerObject2Flip = false;
   var layerObject2Interactive = false;
   var layerObject2Num = 0;
   function Cell()
   {
      super();
   }
   function __get__rootY()
   {
      return this.y - (7 - this.groundLevel) * ank.battlefield.Constants.LEVEL_HEIGHT;
   }
   function addSpriteOnID(sID)
   {
      if(this.allSpritesOn == undefined)
      {
         this.allSpritesOn = new Object();
      }
      if(sID == undefined)
      {
         return undefined;
      }
      if(this.allSpritesOn[sID])
      {
         return undefined;
      }
      this.allSpritesOn[sID] = true;
   }
   function removeSpriteOnID(sID)
   {
      this.allSpritesOn[sID] = undefined;
      delete this.allSpritesOn.sID;
   }
   function removeAllSpritesOnID()
   {
      for(var k in this.allSpritesOn)
      {
         this.allSpritesOn[k] = undefined;
         delete this.allSpritesOn.k;
      }
      delete this.allSpritesOn;
   }
   function __get__spriteOnCount()
   {
      var _loc2_ = 0;
      for(var k in this.allSpritesOn)
      {
         _loc2_ = _loc2_ + 1;
      }
      return _loc2_;
   }
   function __get__spriteOnID()
   {
      if(this.allSpritesOn == undefined)
      {
         return undefined;
      }
      for(var k in this.allSpritesOn)
      {
         if(this.allSpritesOn[k])
         {
            return String(k);
         }
      }
      return undefined;
   }
   function __get__carriedSpriteOnId()
   {
      if(this.allSpritesOn == undefined)
      {
         return false;
      }
      for(var k in this.allSpritesOn)
      {
         if(this.allSpritesOn[k].hasCarriedChild())
         {
            return true;
         }
      }
      return false;
   }
}
