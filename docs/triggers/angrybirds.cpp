let game = obj.game.var;
let x = obj;
let money = me.var.money;

first(x.group == "walls" && me.collision_with(x)){
    game.state = 0;
}
forall(x.group == "money" && me.collision_with(x)){
    money += x.var.value:
    x.kill();
}
forall(x.group("walls") && x.collision_with(obj.border)){
    x.gen.x += game.jumpWalls;
    x.var.size = rand(game.wallMin, game.wallMax);
}

let lvlCup = me.var.levelCup;

first(money >= lvlCup){
    lvlCup *= 1.5;
    let t1 = obj.Toast.clone(1);
    t1.text.label.set("Level up!");
}
