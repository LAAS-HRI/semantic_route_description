 [![License][License-Image]][License-Url]
 [![Dependency Status][Dependency-Image]][Dependency-Url]


### Launch the semantic route description

Fisrt, launch Ontologenius with the semantic_route_description's files:
```
$ roslaunch semantic_route_description route_ontology.launch
```
A graphical user interface will be launched but you don't have to use it.

Now, you can run the semantic_route_description:
```
$ rosrun semantic_route_description description
```

### Call the route description

The services definition is the following:
```
string from
string to
string persona
bool signpost
Route route
---
Route[] routes
float32[] costs
string[] goals
```

The service will give you three arrays in return. All arrays are the same size and give you a route (string array), a cost for each corresponding route and the final location for each corresponding route.

### Persona usage

For the persona parameter, you have six types of persona that you can combine with the `_` delimiter:
 - **lambda** : Average person
    - **knowing** : Person who know the place and will prefer comfort
    - **notKnowing** : Person who knows the place and will prefer the comfort
 - **disabled** : Person who can not take stairs and will need accessibility
    - **disabled_knowing**
    - **disabled_ notKnowing**
 - **young** : A young person who will prefer security
    - **young_knowing**
    - **young_ notKnowing**
 - **old** : Older person who will prefer comfort and safety
    - **old_knowing**
    - **old_ notKnowing**

## Route region usage

---
A route region can start from a place, a path or a region and **must** end at a place.
Usually, we start from pepper_infodesk (or pepper_infodesk_2, ...).

---

To get the route from **pepper_infodesk** to **burger_king**, for an average person, only with regions and interfaces, call the getRouteRegion service as follows:
```
$ rosservice call /semantic_route_description/get_route_region "{from_: 'pepper_infodesk', to: 'burger_king', persona: 'lambda'}"
```

When you make this request, you should have the following result:
```
routes:
  -
    route: [adream_experiment_room, door_C, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_D, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_E, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_h20, adream_hall, elevator_1, first_floor]
  -
    route: [adream_experiment_room, door_h20, adream_hall, stairs_1, first_floor]

costs: [7.2, 7.2, 7.2, 4.16, 6.0]

goals: [burger_king, burger_king, burger_king, burger_king, burger_king]
```

> The same request with a disabled person would have given [inf, inf, inf, 2.6315789222717285, inf] as costs. We can see that the only possible route is the one with the escalator.

### Signpost usage

The signpost parameter can be set to true to allow the route description to find routes that do not go directly to the requested location but that go to a sign bound to that location.
If you use this option, you can use the `goals` result which indicates the final destination of each route.

```
$ rosservice call /semantic_route_description/get_route_region "{from_: 'pepper_infodesk', to: 'burger_king', persona: 'lambda', signpost: true}"
```

When you make this request, you should have the following result:
```
routes:
  -
    route: [adream_experiment_room]
  -
    route: [adream_experiment_room, door_C, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_D, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_E, outside, ff_door_1, first_floor]
  -
    route: [adream_experiment_room, door_h20, adream_hall, elevator_1, first_floor]
  -
    route: [adream_experiment_room, door_h20, adream_hall, stairs_1, first_floor]

costs: [1.0, 7.2, 7.2, 7.2, 4.16, 6.0]

goals: [burger_king_signpost_1, burger_king, burger_king, burger_king, burger_king, burger_king]
```

Here you can notice that a burger king signpost is present in the adream experiment room.

## Route place usage

---
A route place **must** start from a place and **must** end at a place.
Usually, we start from pepper_infodesk (or pepper_infodesk_2, ...).

---

To get the route from **pepper_infodesk** to **burger_king**, call the getRoute service as follows:
```
$ rosservice call /semantic_route_description/get_route "{from_: 'pepper_infodesk', to: 'burger_king', persona: 'lambda'}"
```

When you make this request, you should have the following result:
```
routes:
  -
    route: [robot_infodesk, os_exp_1, gf_ww1_os1_intersection, gf_walkway_1, door_C, outside,
  ff_door_1, ff_corridor_4, burger_king]
  -
    route: [robot_infodesk, os_exp_1, gf_ww2_os1_intersection, gf_walkway_2, door_D, outside,
  ff_door_1, ff_corridor_4, burger_king]
  -
    route: [robot_infodesk, os_exp_1, door_E, outside, ff_door_1, ff_corridor_4, burger_king]
  -
    route: [robot_infodesk, os_exp_1, door_h20, os_hall, elevator_1, ff_corridor_3, ff_c34_intersection,
  ff_corridor_4, burger_king]
  -
    route: [robot_infodesk, os_exp_1, door_h20, os_hall, stairs_1, ff_corridor_3, ff_c34_intersection,
  ff_corridor_4, burger_king]

costs: [12.96, 12.96, 10.08, 7.5, 10.8]

goals: [burger_king, burger_king, burger_king, burger_king, burger_king]
```
Here you have a more precise route that will give you the path where you have to go.

### Signpost usage

As for the route region, you can ask to take signpost in account like it:
```
$ rosservice call /semantic_route_description/get_route "{from_: 'pepper_infodesk', to: 'burger_king', persona: 'lambda', signpost: true}"
```

[License-Url]: https://opensource.org/licenses/Apache-2.0
[License-Image]: https://img.shields.io/badge/License-Apache%202.0-blue.svg
[Dependency-Image]: https://img.shields.io/badge/dependencies-ontologenius-1eb0fc.svg
[Dependency-Url]: https://github.com/sarthou/ontologenius
