//
// This file is part of the UDP blocks toolbox
//
// Copyright (C) 2011 - DIGITEO - Clément DAVID <clement.david@scilab.org>
// see license.txt for more licensing informations

function [x,y,typ]=MAVLINK_RECEIVE(job,arg1,arg2)
  x=[];y=[];typ=[]
  select job
   case 'plot' then
    standard_draw(arg1)
   case 'getinputs' then
    [x,y,typ]=standard_inputs(arg1)
   case 'getoutputs' then
    [x,y,typ]=standard_outputs(arg1)
   case 'getorigin' then
    [x,y]=standard_origin(arg1)
   case 'set' then
    x=arg1;
    // no parameters yet
    model=arg1.model;
    graphics=arg1.graphics;
    values=graphics.exprs;
    while %t do
      [ok, hostPortExpr, timeout, masterID, sysID, compID, newValues]=scicos_getvalue("Set MAVLINK_RECEIVE block parameters", ...
      [_("Port"); ...
       _("Timeout"); ...
       _("Master ID"); ...
       _("System ID"); ...
       _("Component ID")], ...
      list('vec',1, 'vec', 1, 'vec', 1, 'vec', 1, 'vec', 1), ...
      values)
      if ~ok then
        break
      end
      //
      // Port
      //
      if hostPortExpr < 1024
        message("Port number must be greater than 1024.");
        ok=%f;
      else
         hostPort = hostPortExpr;
      end
      if timeout <= 0
          message("Timeout must be stricktly positive.");
          ok=%f;
      end
//      if and(behavior <> [0 1 2])
//          message("Behavior must be 0, 1 or 2")
//          ok = %f;
//      end

      if ok then
        model.ipar = [hostPort ; timeout ; masterID; sysID ; compID];
        graphics.exprs = newValues;
        arg1.graphics = graphics;
        arg1.model = model;
        x=arg1;
        break
      end
    end
   case 'define' then
    model=scicos_model()
    model.sim=list('mavlink_receive',4)
    model.out=[1,1; 1,1; 9,1; 3,1; 3,1; 3,1; 3,1; 3,1; 3,1]
    model.outtyp=[1; 1; 1; 1; 1; 1; 1; 1; 1]
    model.blocktype='c'
    model.dep_ut=[%f %t]
    model.ipar = [14450;1000;250;55;1]

    exprs=string(model.ipar)
    gr_i=[]
    x=standard_define([2 2],model,exprs,gr_i)
    x.graphics.style=["blockWithLabel;verticalLabelPosition=bottom;verticalAlign=top;spacing=0;displayedLabel=Port: %s"];
  end
endfunction
