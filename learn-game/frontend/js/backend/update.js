function square(x){
    return x * x;
}
function food_efficiency(distance){
    return 1.0 / (1.0 + square(2 * distance / FOOD_RADIUS))
}
function take_food(efficiency, unit, food_source, gather_food_action){
    if(gather_food_action > 1){
        console.log("gather_food_action too high: " + gather_food_action)
    }
    var food_taken = efficiency * gather_food_action * food_source.current_ammount
    food_source.current_ammount -= food_taken
    return food_taken
}
function take_food_from_sources(map, cen_unit, gather_food_action){
    return map.get_other_items_in_distance(cen_unit, FOOD_RADIUS)
        .filter(is_food_source_type)
        .map(source => take_food(food_efficiency(map.get_distance(source,cen_unit)), cen_unit, source, gather_food_action))
        .reduce((a, b) => a + b, 0)
}
function rand_line(){
    var CHOICE_RANGE = 0.1
    return (Math.random() * 2 - 1) * CHOICE_RANGE
}
function random_action(){
    var new_action = {
        change_direction_effort: rand_line(),
        movement_effort: rand_line(),
        gather_food_effort: rand_line(),
        eat_food_effort: rand_line() - 1,
    }
    verify_action_set(new_action)
    return new_action
}

function squash_effort_to_positives(effort){
    //effort needs to make sense across the entire real line.
    //This squashes it to another value, "action", which only makes sense in the positive numbers
    var softplus_scale_factor = 0.2;
    return Math.log(1.0 + Math.exp((effort / softplus_scale_factor) - 0.2)) * softplus_scale_factor
}
function squash_effort_to_results(pos_effort){
    return Math.tanh(pos_effort)
}
function effort_strain(stamina, effort){
    return Math.tanh(effort/3) * stamina;
}
function unit_action(map, cur_unit, action_set){
    //direction change
    var prev_direction = cur_unit.current_dir;
    var delta_direction = squash_effort_to_results(action_set.change_direction_effort)
    var dir_effort_pos = Math.abs(action_set.change_direction_effort) * cur_unit.phys_energy
    var direction_strain = effort_strain(cur_unit.phys_energy, dir_effort_pos)
    cur_unit.phys_energy -= direction_strain
    cur_unit.current_dir += delta_direction
    //forward movement
    var movement_positive = squash_effort_to_positives(action_set.movement_effort)
    var movement_action = squash_effort_to_results(movement_positive) * cur_unit.phys_energy
    var movement_strain = effort_strain(cur_unit.phys_energy, movement_positive)
    cur_unit.phys_energy -= movement_strain
    map.move_item(cur_unit, movement_action, prev_direction)
    //attack
    //var attack_positive = squash_effort_to_positives(action_set.attack_effort)
    //map.get_items_along_path(cur_unit,)
    //food collection
    var gath_food_positive = squash_effort_to_positives(action_set.gather_food_effort)
    var gath_food_action = squash_effort_to_results(gath_food_positive) * cur_unit.phys_energy
    var gath_food_strain = effort_strain(cur_unit.phys_energy, gath_food_positive)
    var gath_food_value = take_food_from_sources(map, cur_unit, gath_food_action)
    cur_unit.phys_energy -= gath_food_strain
    cur_unit.food_held += gath_food_value
    //food eating
    //improve physical stamina with food
    var eat_food_positive = squash_effort_to_positives(action_set.eat_food_effort)
    var eat_food_action = squash_effort_to_results(eat_food_positive)
    var food_consumed = cur_unit.food_held * eat_food_action
    cur_unit.phys_energy += food_consumed
    cur_unit.food_held -= food_consumed
}
function unit_time_update(cur_unit){
}
function food_time_update(food_source){
    //update current_ammount to go towards max_ammount over time
    var diff_to_update = food_source.max_ammount - food_source.current_ammount
    var update_val = diff_to_update * 0.01
    food_source.current_ammount += update_val
}
function can_reproduct(unit){
    return unit.food_held > FOOD_REPRODUCE_COST*2
}
function should_die(unit){
    return unit.phys_energy < DEATH_ENERGY
}
function reproduce_unit(map, cur_unit, unique_fact){
    cur_unit.food_held -= FOOD_REPRODUCE_COST
    var new_unit =  Object.assign({},cur_unit)
    new_unit._unique_id = unique_fact.new_unique_id()
    map.place_new_item(new_unit,cur_unit,0,0)
    return new_unit
}
function kill_unit(map, units_list, unit){
    map.remove_item(unit)
    return units_list.filter(item => item._unique_id !== unit._unique_id)
}
function update_game(current_game){
    current_game.units.forEach(unit_time_update)
    current_game.food_sources.forEach(food_time_update)
    current_game.units.forEach(unit => unit_action(current_game.map, unit, current_game.cur_actions[unit._unique_id]))
    for(var i = 0; i < current_game.units.length; i++){
        if(should_die(current_game.units[i])){
            current_game.units = kill_unit(current_game.map, current_game.units, current_game.units[i])
        }
        else if(can_reproduct(current_game.units[i])){
            current_game.units.push(reproduce_unit(current_game.map, current_game.units[i], current_game.unique_factory))
        }
    }
}
function get_random_actions(current_game){
    var rand_actions = {}
    current_game.units.forEach(function(unit){
        rand_actions[unit._unique_id] = random_action()
    })
    return rand_actions;
}
function set_random_actions(current_game){
    set_actions(current_game,get_random_actions(current_game))
}
function set_actions(current_game, actions){
    current_game.cur_actions = actions
}
