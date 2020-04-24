import React from 'react'
import { BrowserRouter as Router, Switch, Route } from 'react-router-dom'
import Logo from './Logo'
import Categories from './Dashboard/Categories'
import Links from './Links'
import Vote from './Survey/Vote'
import Add from './Survey/Add'
import Results from './Survey/Results'
import VoteOrder from './Survey/VoteOrder'
import Explain from './Explain'
import Common from './Common'
import '../style.scss'

export default () => (
	<Router>
		<Switch>
			<Route path="/vote">
				<Common />
				<Vote />
			</Route>
			<Route path="/results">
				<Common />
				<Results />
				<Links />
			</Route>
			<Route path="/add">
				<Common />
				<Add />
			</Route>
			<Route path="/order">
				<Common />
				<VoteOrder />
			</Route>
			<Route path="/findings">
				<Common />
				<Logo />
				<Categories />
				<Links />
			</Route>
			<Route path="/">
				<Common />
				<Logo />
				<Explain />
				<Links />
			</Route>
		</Switch>
	</Router>
)
