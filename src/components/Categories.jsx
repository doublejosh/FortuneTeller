import React, { useState } from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Category from './Category'

export default () => {
	const [votes, setVotes] = useState(0)
	useFirebaseConnect(['categories', 'fortunes'])
	const categories = useSelector(state => state.firebase.data.categories)
	const fortunes = useSelector(state => state.firebase.data.fortunes)

	// fortunes.forEach(f => {
	// 	setVotes(votes + f.votes)
	// })

	return (
		<React.Fragment>
			<section className="stats">{votes}</section>
			<section className="categories">
				{categories &&
					categories.map((category, i) => (
						<Category key={category.id} {...category} />
					))}
			</section>
		</React.Fragment>
	)
}
