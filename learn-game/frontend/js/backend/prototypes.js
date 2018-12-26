food_source_prototype = {
    max_ammount: undefined,
    current_ammount: undefined,
    _unique_id: undefined,
    type: "_food_source",
}
unit_prototype = {
    current_dir: undefined,
    phys_energy: undefined,
    //health: undefined,
    //mental_energy: undefined,
    //is_asleep: undefined,
    food_held: undefined,
    //phys_stamina: undefined,
    //max_health: undefined,
    //combat_ablity: undefined,
    _unique_id: undefined,
    type: "_unit",
}
action_set_proto = {
    change_direction_effort: undefined,
    movement_effort: undefined,
    //attack_effort: undefined,
    gather_food_effort: undefined,
    eat_food_effort: undefined,
    //take_food_proportion: undefined,
    //give_food_proportion: undefined,
}
function verify_instance_of_proto(instance, proto){
    for(k in instance){
        if(!(k in proto)){
            console.assert(k in proto, "key "+k+" in instance but not in proto");
        }
    }
    for(k in proto){
        if(!(k in instance)){
            console.assert(k in instance, "key "+k+" in prototype but not instance");
        }
    }
    Object.values(instance).forEach(function(val){
        if(val === undefined){
            console.assert(!(val === undefined), "some value is undefined in object "+JSON.stringify(instance));
        }
    })
}
function verify_unit(new_u){
    verify_instance_of_proto(new_u,unit_prototype)
    console.assert(new_u.type === unit_prototype.type)
}
function verify_food_source(new_food_source){
    verify_instance_of_proto(new_food_source,food_source_prototype)
    console.assert(new_food_source.type === food_source_prototype.type)
}
function verify_action_set(new_action_set){
    verify_instance_of_proto(new_action_set,action_set_proto)
}
function is_unit_type(object){
    return object.type == unit_prototype.type
}
function is_food_source_type(object){
    return object.type == food_source_prototype.type
}
