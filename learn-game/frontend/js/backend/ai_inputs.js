/*
AI input format:

1. All values must be between -1 and 1.
2. Output size must be fixed no matter what the map or the unit is like
*/
function ai_input_dict_to_vector(ai_dict){
    var res_list = []
    res_list.push(ai_dict.unit_info.phys_energy)
    res_list.push(ai_dict.unit_info.food_held)
    ai_dict.sight_info.forEach(function(buck_item){
        res_list.push(buck_item.is_something_there)
        res_list.push(buck_item.is_unit)
    })
    return res_list
}
function get_ai_inputs(game_state){
    var total_list = []
    game_state.units.forEach(function(unit){
        var raw_sight_buckets = game_state.map.get_sight_ai_buckets(unit)
        var proc_sight_buckets = process_sight_buckets(raw_sight_buckets)

        var procced_data = {
            unit_info: get_unit_ai_info(unit),
            sight_info: proc_sight_buckets
        };
        total_list[unit._unique_id] = ai_input_dict_to_vector(procced_data)
    })
    return total_list
}
function get_unit_ai_info(unit){
    return {
        phys_energy: unit.phys_energy,
        food_held: unit.food_held,
    }
}
function process_sight_buckets(sight_buckets){
    return sight_buckets.map(function(raw_bucket_item){
        var see_item = get_closest_item_from_bucket(raw_bucket_item)
        var proc_bucket = get_item_ai_info(see_item)
        return proc_bucket;
    })
}
function get_closest_item_from_bucket(bucket_of_items){
    return bucket_of_items.length == 0 ? null : bucket_of_items[0];
}
function get_item_ai_info(item){
    if(item == null){
        return {
            is_something_there : 0,
            is_unit : 0,
        }
    }
    else{
        return {
            is_something_there : 1,
            is_unit : is_unit_type(item) ? 1 : 0,
        }
    }
}
