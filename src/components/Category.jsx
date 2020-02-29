import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Fortune from './Fortune'

// const fortuneList = state => state.firebase.data.fortunes
// const categoryId = state => state.id
// export const filteredFortunes = createSelector(
// 	[fortuneList, categoryId],
// 	(fortunes, id) => fortunes.filter(f => f.category === id)
// )

export default props => {
	useFirebaseConnect([
		{
			path: 'fortunes',
		},
	])

	const getScore = (votes, shown) => {
		if (shown === 0 || votes > shown) return 0
		return Math.round((parseInt(votes) / parseInt(shown)) * 100)
	}

	// return list.map(f => {
	// 	return { ...f, score: getScore(f.votes, f.shown) }
	// })

	const fortunes = useSelector(state => state.firebase.data.fortunes)
	let totalShown = 0,
		totalVotes = 0

	return (
		<section className={'category ' + props.id}>
			<h3>{props.label}</h3>
			<ul className="fortunes">
				{fortunes &&
					fortunes
						.filter(f => f.category === props.id)
						.map(f => ({ ...f, score: getScore(f.votes, f.shown) }))
						.sort((a, b) => b.score - a.score)
						.map((f, i) => {
							totalShown += f.shown
							totalVotes += f.votes
							return <Fortune key={i} {...f} />
						})}
			</ul>
			<div className="count-badge">
				{totalVotes} / {totalShown}
				{' (' + Math.round((totalVotes / totalShown) * 100)}%)
			</div>
		</section>
	)
}
