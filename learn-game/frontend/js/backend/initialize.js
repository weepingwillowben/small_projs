function create_random_food_source(game){
    var new_food_source = {
        max_ammount: Math.random(),
        current_ammount: 0,
        _unique_id: game.unique_factory.new_unique_id(),
        type: food_source_prototype.type,
    }
    new_food_source.current_ammount = new_food_source.max_ammount
    game.map.place_new_item_randomly(new_food_source)
    verify_food_source(new_food_source)
    return new_food_source
}
function create_random_unit(game){
    var new_unit = {
        current_dir: Math.random()*8,
        phys_energy: 1+Math.random(),
        //health: Math.random(),
        //mental_energy: Math.random(),
        //is_asleep: false,
        food_held: 0,
        //phys_stamina: 1+Math.random(),
        //max_health: 1+Math.random(),
        //combat_ablity: 1+Math.random(),
        _unique_id: game.unique_factory.new_unique_id(),
        type: unit_prototype.type,
    };
    game.map.place_new_item_randomly(new_unit)
    verify_unit(new_unit)
    return new_unit
}
function initialize_units(game){
    var num_units = 30;
    //console.log(create_random_unit(game))
    return Array(num_units).fill(0).map(x => create_random_unit(game));
}
function initialize_food(game){
    var num_food = 80;
    return Array(num_food).fill(0).map(x => create_random_food_source(game));
}
function initialize_game(){
    game = {
        map: new GameMap(),
        unique_factory: new UniqueFactory(),
    }
    game.food_sources = initialize_food(game)
    game.units = initialize_units(game)
    return game
}
