#include "fairdicegame.hpp"

void fairdicegame::reveal(const uint64_t& id, const checksum256& seed) {
    require_auth(REVEALER);
    st_bet bet = find_or_error(id);
    assert_seed(seed, bet.seed_hash);

    uint8_t random_roll = compute_random_roll(seed, bet.user_seed_hash);
    asset payout = asset(0, EOS_SYMBOL);
    if (random_roll < bet.roll_under) {
        const asset offer = compute_offer(bet.amount, bet.referrer);
        payout = compute_payout(bet.roll_under, offer);
        action(permission_level{_self, N(active)},
               N(eosio.token),
               N(transfer),
               make_tuple(_self, bet.player, payout, winner_memo(bet)))
            .send();
    }

    if (is_valid_referrer(bet.referrer)) {
        const asset referrer_reward = compute_referrer_reward(bet.amount);
        // defer trx, no need to rely heavily
        transaction trx;
        trx.actions.emplace_back(permission_level{_self, N(active)},
                                 N(eosio.token),
                                 N(transfer),
                                 make_tuple(
                                     _self,
                                     bet.referrer,
                                     referrer_reward,
                                     referrer_memo(bet)));

        trx.send(bet.id, _self, false);
    }

    unlock(bet.amount);

    remove(bet);
    st_result result{.bet_id = bet.id,
                     .player = bet.player,
                     .referrer = bet.referrer,
                     .amount = bet.amount,
                     .roll_under = bet.roll_under,
                     .random_roll = random_roll,
                     .seed = seed,
                     .seed_hash = bet.seed_hash,
                     .user_seed_hash = bet.user_seed_hash,
                     .payout = payout};
    action(permission_level{_self, N(active)},
           LOG,
           N(result),
           result)
        .send();
}

void fairdicegame::transfer(const account_name& from,
                            const account_name& to,
                            const asset& quantity,
                            const string& memo) {
    if (from == _self || to != _self) {
        return;
    }

    uint8_t roll_under;
    checksum256 seed_hash;
    checksum160 user_seed_hash;
    uint64_t expiration;
    account_name referrer;
    signature sig;

    parse_memo(memo, &roll_under, &seed_hash, &user_seed_hash, &expiration, &referrer, &sig);

    //check quantity
    assert_quantity(quantity);

    //check roll_under
    assert_roll_under(roll_under, compute_offer(quantity, referrer));

    //check seed hash && expiration
    assert_hash(seed_hash, expiration);

    //check referrer
    eosio_assert(referrer != from, "referrer can not be self");

    //check signature
    assert_signature(roll_under, seed_hash, expiration, referrer, sig);

    const st_bet _bet{.id = next_id(),
                      .player = from,
                      .referrer = referrer,
                      .amount = quantity,
                      .roll_under = roll_under,
                      .seed_hash = seed_hash,
                      .user_seed_hash = user_seed_hash,
                      .created_at = now()};
    save(_bet);
    lock(quantity);
    action(permission_level{_self, N(active)},
           _self,
           N(receipt),
           _bet)
        .send();
}

void fairdicegame::receipt(const st_bet& bet) {
    require_auth(_self);
}
