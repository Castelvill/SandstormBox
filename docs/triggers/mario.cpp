let entity = obj;
let timer = me.var.timer;

each_second{
    timer++;
    if(timer == 5){
	say("5 seconds have passed")
        timer = 0;
    }
}

forall(me.collisionWith(entity)){
    first(entity.inGroup("enemy")){
        me.var.life -= 1;
        let gameVar = layer.master.game.var;
        if(me.var.life > 0){
            layer.current.transform_into(layer(gameVar.currentWorld));
            me.x = gameVar.lastCheckpointX;
            me.y = gameVar.lastCheckpointY;
        }
        else{
            layer.current.transform_into(layer(gameVar.menu));
        }
    }

    first(entity.inGroup("mushroom")){
        execute("power_up");
    }

    forall(entity.inGroup("money")){
        me.var.money += 1;
        entity.kill();
    }
}


