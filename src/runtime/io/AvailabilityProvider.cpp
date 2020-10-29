#include "AvailabilityProvider.hpp"
AvailabilityProvider::CompletableFuturePtr AvailabilityProvider::AVAILABLE = CompletableFuture<bool>::complete_future(true);
