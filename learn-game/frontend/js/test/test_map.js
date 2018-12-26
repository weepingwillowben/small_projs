
function close_enough(f1,f2){
    return Math.abs(f1-f2) < 0.000001
}
function get_ids(item_arr){
    return Object.values(item_arr).map(item => item._unique_id)
}
function arrays_equal(arr1,arr2){
    return get_ids(arr1).sort().join(",") === get_ids(arr2).sort().join(",")
}
class TestItem{
    constructor(unique_id){
        this._unique_id = unique_id
    }
}
function test_place_rand(){
    var map = new GameMap()
    var unique_fact = new UniqueFactory()
    var item_rand1 = new TestItem(unique_fact.new_unique_id())
    var item_rand2 = new TestItem(unique_fact.new_unique_id())
    map.place_new_item_randomly(item_rand1)
    map.place_new_item_randomly(item_rand2)
    console.assert(30 > map.get_distance(item_rand1,item_rand2))
    console.assert(arrays_equal(map.get_other_items_in_distance(item_rand1,map.get_distance(item_rand1,item_rand2)+0.001),[item_rand2]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item_rand1,map.get_distance(item_rand1,item_rand2)-0.001),[]))
}
function test_get(){
    var map = new GameMap()
    var unique_fact = new UniqueFactory()

    var item1 = new TestItem(unique_fact.new_unique_id())
    var item1_far = new TestItem(unique_fact.new_unique_id())
    var item1_1 = new TestItem(unique_fact.new_unique_id())
    var item1_2 = new TestItem(unique_fact.new_unique_id())
    var item1_3 = new TestItem(unique_fact.new_unique_id())
    var item1_n1 = new TestItem(unique_fact.new_unique_id())
    var item1_n2 = new TestItem(unique_fact.new_unique_id())
    map._place_new_item_at(item1,{x:5,y:5})// a little white box here, unfortunately...
    map.place_new_item(item1_far,item1,1000000,0)
    map.place_new_item(item1_1,item1,1,1)
    map.place_new_item(item1_2,item1,2,1)
    map.place_new_item(item1_3,item1,3,1)
    map.place_new_item(item1_n1,item1,1,1.5)
    map.place_new_item(item1_n2,item1,-1,1.5+Math.PI)
    console.assert(close_enough(0,map.get_distance(item1_n1,item1_n2)))
    console.assert(close_enough(2,map.get_distance(item1_1,item1_3)))
    console.assert(30 > map.get_distance(item1,item1_far))
    console.assert(30 > map.get_distance(item1_2,item1_far))
    console.assert(arrays_equal(map.get_other_items_in_distance(item1,1.01),[item1_1,item1_n1,item1_n2]))
    console.assert(!arrays_equal(map.get_other_items_in_distance(item1,1.01),[item1_1,item1_n1,item1_n2,item1_2]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item1_n1,0.9),[item1_n2,item1_1]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item1_2,1.01),[item1_1,item1_3]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item1_2,30),[item1_1,item1_3,item1,item1_far,item1_n1,item1_n2]))
}

function test_move(){
    var map = new GameMap()
    var unique_fact = new UniqueFactory()

    var item1 = new TestItem(unique_fact.new_unique_id())
    var item2 = new TestItem(unique_fact.new_unique_id())
    map.place_new_item_randomly(item1)// a little white box here, unfortunately...
    map.place_new_item(item2,item1,1,3)
    console.assert(close_enough(1,map.get_distance(item1,item2)))
    map.move_item(item2,1,3)
    console.assert(close_enough(2,map.get_distance(item1,item2)))
    console.assert(arrays_equal(map.get_other_items_in_distance(item1,2+0.01),[item2]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item2,2+0.01),[item1]))
    console.assert(arrays_equal(map.get_other_items_in_distance(item2,2-0.01),[]))

}
function test_remove(){
    var map = new GameMap()
    var unique_fact = new UniqueFactory()

    var item1 = new TestItem(unique_fact.new_unique_id())
    var item2 = new TestItem(unique_fact.new_unique_id())
    map.place_new_item_randomly(item1)// a little white box here, unfortunately...
    map.place_new_item(item2,item1,1,3)
    console.assert(close_enough(1,map.get_distance(item1,item2)))
    map.remove_item(item2)
    console.assert(arrays_equal(map.get_other_items_in_distance(item1,200),[]))
}
function test_map(){
    test_place_rand()
    test_get()
    test_move()
    test_remove()
    console.log("all tests in test_map.js passed")
}
test_map()
