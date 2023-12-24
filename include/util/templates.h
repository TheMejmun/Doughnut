//
// Created by Sam on 2023-10-26.
//

#ifndef DOUGHNUT_TEMPLATES_H
#define DOUGHNUT_TEMPLATES_H

namespace Doughnut {
    template<class T, class... OTHER>
    struct FirstOf {
        using Type = T;
    };

    template<class... T>
    struct TupleOrSingle {
        using Type = typename std::conditional<(sizeof...(T) > 1),
                std::tuple<T...>,
                typename FirstOf<T...>::Type
        >::type;
    };
}

#endif //DOUGHNUT_TEMPLATES_H
