
class UniqueFactory{
    constructor(){
        this.next_id = 1;
    }
    new_unique_id(){
        var this_unique_id = this.next_id
        this.next_id += 1
        return this_unique_id;
    }
}
