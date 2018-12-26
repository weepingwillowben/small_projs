/*
The goal is to have all distance and location information abstracted by the
map class.

To all other items, everything is in radial notation (angle, distance) from itself,

Only the map should know about absolute position. This will allow us to write
signficant optimizations to speed up map operations.

The plan for the optimized map is this:

Cut board into large number of blocks, say, 10000 blocks. Each block stores the
items inside it. Search only happens in blocks that have some area within the
range of the search.
*/
class GameMap {
    constructor() {
        this.width = 20;
        this.height = 10;
        this.id_locations = {}
    }
    place_new_item(item, center_item, distance, angle){
        var base_loc = euclidian_location(this._get_location(center_item), distance, angle)
        this._place_item_at(item,base_loc)
    }
    place_new_item_randomly(item){
        var random_location = {
            x: this.width * Math.random(),
            y: this.height * Math.random(),
        }
        this._place_item_at(item,random_location)
    }
    move_item(item, distance, angle){
        var new_loc = euclidian_location(this._get_location(item), distance, angle)
        this.remove_item(item)
        this._place_item_at(item, new_loc)
    }
    remove_item(item){
        delete this.id_locations[item._unique_id]
    }
    get_other_items_in_distance(center_item, dist){
        var center_location = this._get_location(center_item)
        //TODO: this procedure can be very slow if map l,mis large. optimize this if it is a problem
        return Object.values(this.id_locations)
                .filter(loc_item => (distance(center_location,loc_item.loc) < dist))
                .map(loc_item => loc_item.item)
                .filter(loc_item => loc_item._unique_id != center_item._unique_id)
    }
    get_sight_ai_buckets(ai_unit){
        /*
        OUTPUT SPECIFICATION: As an AI input, this must have the following properties

        1. All values must be between -1 and 1.
        2. Output size must be fixed no matter what the map is like

        see ai_inputs.js for more info
        */
        var map = this
        verify_unit(ai_unit)

        //get items in radial view, putting the items in buckets for each possbile direction
        var sight_buckets = new Array(NUMBER_SIGHT_SPLITS);
        for(var i = 0; i < NUMBER_SIGHT_SPLITS; i++){
            sight_buckets[i] = [];
        }
        this.get_other_items_in_distance(ai_unit, MAX_SIGHT_DIST).forEach(function(item){
            var dir_offset = normify_angle(ai_unit.current_dir - angle_from_to(map._get_location(ai_unit),map._get_location(item)))
            var bucket_num = dir_offset * (NUMBER_SIGHT_SPLITS / (2*Math.PI))
            sight_buckets[Math.floor(bucket_num)].push(item)
        })
        return sight_buckets
    }
    get_items_along_path(center_item, dist, dir, other_items_rad){
        var start_loc = this._get_location(center_item)
        var end_loc = euclidian_location(start_loc)
        return this.get_other_items_in_distance(center_item, dist + other_items_rad)
            .filter(loc_item => dist_to_path(start_loc,end_loc,loc_item.loc) < other_items_rad)
    }
    get_distance(item1, item2){
        return distance(this._get_location(item1),this._get_location(item2))
    }
    _get_loc_item(item){
        return this.id_locations[item._unique_id]
    }
    _get_location(item){
        return this.id_locations[item._unique_id].loc
    }
    _place_item_at(item, location){
        console.assert(!(item._unique_id in this.id_locations))
        var contained_loc = wrap_loc(this.width, this.height, location)
        this.id_locations[item._unique_id] = {loc:contained_loc, item: item}
    }
    get_item_locations(scale_factor){
        return Object.values(this.id_locations)
            .map(function(loc_item){ return {
                loc: vec_scalar_mult(loc_item.loc,scale_factor),
                item: loc_item.item,
            }})
    }
}


//
