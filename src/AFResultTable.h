#ifndef H_AFRESULTTABLE_H
#define H_AFRESULTTABLE_H

///
/// \author John Farrier, Brian Kloppenborg
///
/// \copyright Copyright 2014 John Farrier , Brian Kloppenborg
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
/// http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

#include <celero/Pimpl.h>
#include <celero/Experiment.h>
#include <string>

class AFResultsTable
{
    public:
        ///
        /// Singleton
        ///
        static AFResultsTable& Instance();

        ///
        /// Specify a file name for a results output file.
        ///
        /// \param x    The name of the output file in which to store Celero's results.
        ///
        void setFileName(const std::string& x);

        ///
        /// Adds an extra column that will appear in every row in the table
        ///
        /// \param header The label that will appear in the header row
        /// \param value  The value that will appear in every data row
        void addStaticColumn(const std::string& header, const std::string& value);

        ///
        /// Add a new result to the result table.
        ///
        /// This should re-save on every new result so that the output can be monitored externally.
        ///
        void add(std::shared_ptr<celero::Result> x);

        ///
        ///
        ///
        void save();

    private:
        ///
        /// Default Constructor
        ///
        AFResultsTable();

        ///
        /// Default Destructor
        ///
        ~AFResultsTable();

        ///
        /// \brief  Pimpl Idiom
        ///
        class Impl;

        ///
        /// \brief  Pimpl Idiom
        ///
        celero::Pimpl<Impl> pimpl;
};

#endif
