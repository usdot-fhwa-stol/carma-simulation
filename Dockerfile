#  Copyright (C) 2018-2020 LEIDOS.
# 
#  Licensed under the Apache License, Version 2.0 (the "License"); you may not
#  use this file except in compliance with the License. You may obtain a copy of
#  the License at
# 
#  http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#  License for the specific language governing permissions and limitations under
#  the License.

FROM usdotfhwastoldev/carma-base:develop as deps

# Install remaining package deps
RUN mkdir ~/src
COPY --chown=carma . /home/carma/src/
RUN ~/src/docker/checkout.sh
RUN ~/src/docker/install.sh

FROM deps

RUN mkdir ~/src
COPY --chown=carma . /home/carma/src/
RUN ~/src/docker/checkout.sh
RUN ~/src/docker/install.sh

FROM deps

ARG BUILD_DATE="NULL"
ARG VERSION="NULL"
ARG VCS_REF="NULL"

LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="carla-ackermann-control-wrapper-driver"
LABEL org.label-schema.description="carla ackermann control wrapper driver for the CARMA Platform"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version=${VERSION}
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations/CARMA"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-simulation/"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}

COPY --from=setup /home/carma/install /opt/carma/install

CMD [ "wait-for-it.sh", "localhost:11311", "--", "roslaunch", "carla_ackermann_control", "carla_ackermann_control_wrapper.launch"]