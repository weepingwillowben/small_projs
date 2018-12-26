* Map
    * Continuous finite map, with torus borders (Implemented)
    * y/(1+d^2) bump concentrations of food productivity spread over the map randomly (Implemented)
        * y value decreases when people gather food (Implemented)
        * y values goes to max food value over time (Implemented)
    * Starting people spread randomly (Implemented)
* Choices
    * Principles of choices:
        * All inputs need to make sense across the whole real line.
        * All actions except for radial movement are forward only
    * Radial movement  (Implemented)
        * input
            * direction-effort: both direction and effort, large negative numbers are left, large positives are right.
    * Forward Movement: (Implemented)
        * input:
            * effort (translates to distance)
    * Combat: (Not Implemented)
        * input
            * effort (effort translates to things like range and attack magnitude)
    * Gather food  (Implemented)
        * inputs:
            * effort
        * modifiers
            * how much food there is to be gathered at that map location
    * Eat food (Not implemented)
        * inputs:
            * effort
        * modifiers
            * Global max-eat value
    * Take food (Not Implemented)
        * input:
            * Percent available food
        * Is accepted when:
            * Person is unconscious/asleep and person is in range
    * Give food: (Not Implemented)
        * Input:
            * Percent available food
* Consciousness status:
    * Sleep (Not Implemented)
        * increases physical energy recovery
        * increases health recovery
        * increases mental energy recovery
        * no choices
        * no observation
        * others can initiate "take food"
    * Awake
* Basic stats (stats that change quickly over time)
    * Direction (Implemented)
        * deltas
            * Radial movement
        * effects:
            * All actions at a distance: (Take food, give food, combat, forward movement) happen in this direction
            * AI input is dependent on direction (Not implemented)
    * Physical energy: (Implemented)
        * effects:
            * dies when too low
        * deltas:
            * Movement/combat effort decreases it
            * Increases as much as food is decreased
            * Sleep/Unconsciousness accelerates increase
    * Food held (Implemented)
        * effects:
            * When decreased, physical energy increases
        * deltas:
            * Food increases
    * Health: (Not Implemented)
        * state:
        * effects:
            * If health is below 20%, drop unconscious.
            * If health falls below 10%, physical stamina and max health decreases
    * Mental energy  (Not Implemented)
        * deltas:
            * decreases over time
            * Sleep increases it
            * Unconsciousness keeps it the same
        * effects:
            * lower values lowers combat ability
* Identity stats (stats that change very slowly over time)
    * physical stamina: (Not Implemented)
        * effects: increases physical energy max and rate
    * max health (Not Implemented)
        * increases health max and rate
* Birth stats (stats that are set only when born)
    * physical_size: (Not Implemented)
        * effects:
* Globals (environment stats that are always fixed for everyone)
    *
